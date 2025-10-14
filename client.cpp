#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>
#include <unistd.h>
#include <ncurses.h>
#include <variant>

#include "vector.h"
#include "player.h"
#include "bomb.h"
#include "time.h"

const char *ServerIp = "192.168.1.120";
int ServPort = 10;

using namespace std;

enum
{
    AffectedArea = 15,
    AffectedAreaXCoefficient = 2,
    AffectedAreaYCoefficient = 1,
};

typedef enum 
{
    firstStation = 1,
    SecondStation = 2,
    ThirdStation = 3,
    lastStation = 4,
}stations;

typedef enum
{
    NothingColor    = 1,
    PlayerColor     = 2,
    BombColor       = 3,
    HPColor         = 4,
}colors;

struct sockaddr_in FillAddr(struct sockaddr_in ServAddr, const char *ip, int ServPort)
{
  ServAddr.sin_family = AF_INET;
     ServAddr.sin_port = htons(ServPort);
    if (!inet_aton(ip, &(ServAddr.sin_addr)))
  {
        printf("ошибка: %d", errno);
        errno = -1;
    return ServAddr;
  }
  return ServAddr;
}

int CreateAndConnectTo(struct sockaddr_in ServAddr)
{
  int sd;
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
        printf("ошибка: %d", errno);
    return -1;
  }

    if (-1 == (connect(sd, (struct sockaddr *)&ServAddr, sizeof(ServAddr))))
    {   
        printf("ошибка: %d", errno);
        return -1;
    }
  return sd;
}

bool explode(int BombPositionY, int BombPositionX, Vector PositionBorders, int waitingTime, int ExplodeColor)       
{
    attrset(COLOR_PAIR(ExplodeColor));
    static bool BombExploded;
    static stations station = firstStation;
    switch (station)
    {
    case firstStation: 
        for(int i = 0; i < AffectedArea; i++)
        {
            for(int x = BombPositionX-i*AffectedAreaXCoefficient; x < BombPositionX+i*AffectedAreaXCoefficient; x++)
            {
                if((x>0) && (x<PositionBorders.x))
                {
                    for(int y = BombPositionY-i*AffectedAreaYCoefficient; y < BombPositionY+i*AffectedAreaYCoefficient; y++)
                    {
                        if( (y>0) && (y<PositionBorders.y))
                        {
                            move(y, x);
                            addch('*');
                        }
                    }
                }
            }
        }
        station = SecondStation;
        BombExploded = false;
        break;
    case SecondStation: 
        if(true == stopwatch(waitingTime, time(NULL), 0))       
        {
            station = ThirdStation;
        }
        BombExploded = false;
        break;
    case ThirdStation:         
        for(int i = 0; i < AffectedArea; i++)
        {
            for(int x = BombPositionX-i*AffectedAreaXCoefficient; x < BombPositionX+i*AffectedAreaXCoefficient; x++)
            {
                if((x>0) && (x<PositionBorders.x))
                {
                    for(int y = BombPositionY-i*AffectedAreaYCoefficient; y < BombPositionY+i*AffectedAreaYCoefficient; y++)
                    {
                        if( (y>0) && (y<PositionBorders.y))
                        {
                            move(y, x);
                            addch(' ');
                        }
                    }
                }
            }
        }
        BombExploded = false;
        station = lastStation;
        break;
    case lastStation: 
        BombExploded = true;
        station = firstStation;
        break;
    default:
        break;
    }
    refresh();
    return BombExploded;
}

int SetFdss(int fd1, int fd2, fd_set &readfds)
{
  FD_ZERO(&readfds);
  FD_SET(fd1, &readfds);
    FD_SET(fd2, &readfds);
  return 0;
}

void StartWindow()
{
    initscr();
    cbreak();
    keypad(stdscr, 1);
    noecho();
    curs_set(0);
}
int main()
{    
    colors Color;
    struct timeval timeout;
    object MessangeFrom; 
    Vector PositionBorders;
    object Object;
    player Player;
    bomb Bomb;
    bool MustSend = false, bombExploding = false, MustShowObject;
    int sd, MaxD, SelRes, ReadBytes, key;
    struct sockaddr_in ServAddr;
    fd_set readfds;
    FD_ZERO(&readfds);
    
    ServAddr = FillAddr(ServAddr, ServerIp, ServPort);
    if (errno == -1)
    {
        return -1;
    }
    
    if ((sd = CreateAndConnectTo(ServAddr)) == -1)
    {
        return -1;
    }

    if (0 > (ReadBytes = read(sd, &MessangeFrom, sizeof(MessangeFrom))))
    {   
        printf( "while connect read error:%d\n", errno);
        return(-1);
    }
    
    MaxD = sd;

    StartWindow();
    start_color();

    init_pair(NothingColor, COLOR_BLACK, COLOR_BLACK);
    init_pair(PlayerColor, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BombColor, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(HPColor, COLOR_RED, COLOR_BLACK);

    getmaxyx(stdscr, PositionBorders.y, PositionBorders.x);

    PositionBorders/2;

    Player.setStatue(alive);
    Player.setPosition(PositionBorders.y/2, PositionBorders.x/2);
    Object = Player;
    MustShowObject = true;
    MustSend = true;
    //начало бесконечного цыкла

    while (true)
    {
        SetFdss(sd, STDIN_FILENO, readfds);
        timeout.tv_usec = 333333;
        if ((SelRes = select(MaxD+1, &readfds, NULL, NULL, &timeout)) < 0)
        {
            if (errno != EINTR)
            {
                printf("ошибка select: %d\n", errno);
                break;
            }else
            {
                printf("пришёл неигнорируемый сигнал: %d\n", errno);
                break;
            }
            continue;
        }else if (SelRes > 0)
        {
            //общение с клиентом:
            if(FD_ISSET(STDIN_FILENO, &readfds))
            {

                if (10 != (key = getch()))
                {   
                    switch (key)
                    {
                    case KEY_UP:
                        if(Player.GetY()-1 > 0)
                        {
                            Player.GetY()--;
                            MustSend = true;
                        }
                        break;
                    case KEY_RIGHT:
                        if(Player.GetX()+DefaultWidth+1< PositionBorders.x)
                        {
                            Player.GetX()++;
                            MustSend = true;
                        }
                        break;
                    case KEY_LEFT:
                        if(Player.GetX()-1 > 0)
                        {
                            Player.GetX()--;
                            MustSend = true;
                        }
                        break;
                    case KEY_DOWN:
                        if(Player.GetY()+DefaultHigh+1 < PositionBorders.y)
                        {
                            Player.GetY()++;
                            MustSend = true;
                        }
                        break;
                    default:
                        break;
                    } 
                    if(MustSend)
                    {
                        Player.setStatue(alive);
                        Object = Player;
                        MustShowObject = true;
                    }
                }else break;
            }

            //конец

            //начало прямого общения с сервиром:
            if(FD_ISSET(sd, &readfds))
            {
                if (0 > (ReadBytes = read(sd, &MessangeFrom, sizeof(MessangeFrom))))
                {   
                    printf( "read error:%d\n", errno);
                    break;
                }
                else if(ReadBytes == 0)
                {
                    printf("novogo goda ne bydet, idi nahyi\n");
                    break;
                }
                Object = MessangeFrom;
                MustShowObject = true;
            }
        }else if(SelRes == 0)
        {
            Player.setStatue(alive);
            Object = Player;
            MustShowObject = true;
            timeout.tv_usec = 333333;
        } 
        if(MustShowObject)
        {
            Object.Hide(NothingColor);
            if (Object.getStatue() == active) 
            {
                if (Object.getType() == PlayerType)
                {
                    Color = PlayerColor;
                    Player.showHP(Player.GetY()-1, Player.GetX()-1, HPColor);
                }else if (Object.getType() == BombType)
                {
                    Color = BombColor;
                }
                Object.Show(Color);
                mvprintw(0, 0 ,"");      
            }else if (Object.getType() == BombType)
            {
                Bomb.setPosition(Object.GetY(), Object.GetX());
                bombExploding = !explode(Bomb.GetY(), Bomb.GetX(), PositionBorders, 1, BombColor);  
                if(Player.GetX() > Bomb.GetX() - AffectedArea*AffectedAreaXCoefficient)
                {
                    if(Player.GetX() < Bomb.GetX() + AffectedArea*AffectedAreaXCoefficient)
                    {
                        if(Player.GetY() > Bomb.GetY() - AffectedArea*AffectedAreaYCoefficient)
                        {
                            if(Player.GetY() < Bomb.GetY() + AffectedArea*AffectedAreaYCoefficient)
                            {
                                Player.GetHP()--;
                            }
                        }
                    }
                }
            }
            MustShowObject = false;
        }

        if(Player.GetHP() == 0)
        {
            break;
        }

        if(bombExploding)
        {
            bombExploding = !explode(Bomb.GetY(), Bomb.GetX(), PositionBorders, 1, BombColor);
        }

        if (MustSend)
        {
            if(write(sd, &Player, sizeof(Player)) == -1)
            {
                printf("ошибка: %d", errno);
                break;
            }
            MustSend = false;
        } 
        //конец
        refresh();
    }
    close(sd);
    endwin();
    return -1;
}