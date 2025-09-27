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


using namespace std;

enum
{
    AffectedArea = 15,
    AffectedAreaXCoefficient = 2,
    AffectedAreaYCoefficient = 1
};

const char *ServerIp = "192.168.1.120";
int ServPort = 10;

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

bool explode(int BombPositionY, int BombPositionX, Vector PositionBorders)
{
    static int x; 
    static int y;
    static int i;
    static bool firstExecution = true;
    static float waitingTime;
    if(firstExecution)
    {
        x = BombPositionX-i*AffectedAreaXCoefficient; 
        y = BombPositionY-i*AffectedAreaYCoefficient;
        i = 0;
        waitingTime = 1;
        firstExecution = false;
    }
    if(!firstExecution)
    {
        while(i < AffectedArea)
        {
            while(x <= BombPositionX+i*AffectedAreaXCoefficient)
            {
                if((x>0) && (x<PositionBorders.x))
                {
                    while(y <= BombPositionY+i*AffectedAreaYCoefficient )
                    {
                        if( (y>0) && (y<PositionBorders.y))
                        {
                            move(y, x);
                            addch('*');
                            
                        }
                        y++;
                    }
                }
                x++;
            }
            refresh();
            if(x >= BombPositionX+i*AffectedAreaXCoefficient)
            {
                i++;
                x = BombPositionX-i*AffectedAreaXCoefficient; 
                y = BombPositionY-i*AffectedAreaYCoefficient;
            }
            return firstExecution;
        }
        /*while(false == stopwatch(waitingTime, time(NULL)))
        {
            waitingTime -=0.5;
            return firstExecution;
        }*/
        sleep(1);
        while(i < AffectedArea)
        {
            while(x <= BombPositionX+i*AffectedAreaXCoefficient)
            {
                if((x>0) && (x<PositionBorders.x))
                {
                    while(y <= BombPositionY+i*AffectedAreaYCoefficient)
                    {
                        if( (y>0) && (y<PositionBorders.y))
                        {
                            move(y, x);
                            addch(' ');
                            
                        }
                        y++;
                    }
                }
                x++;
            }
            refresh();
            if(x >= BombPositionX+i*AffectedAreaXCoefficient)
            {
                i++;
                x = BombPositionX-i*AffectedAreaXCoefficient; 
                y = BombPositionY-i*AffectedAreaYCoefficient;
            }
            return firstExecution;
        }
    }
    firstExecution = true;                  
    return firstExecution;
}

int SetFdss(int fd, fd_set &readfds)
{
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);
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
    struct timeval timeout;
    object MessangeFrom; 
    std::variant<Vector, int> messangeFor; 
    Vector PositionBorders, position;
    object Object(5, 2, position);
    bool MustSend = false, bombExploding = false;
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

    getmaxyx(stdscr, PositionBorders.y, PositionBorders.x);
    messangeFor = (Vector){PositionBorders.x - 2, PositionBorders.y - 5}; 
    MustSend = true;

    //начало бесконечного цыкла

    while (true)
    {
        SetFdss(sd, readfds);
        FD_SET(STDIN_FILENO, &readfds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 1;
        if ((SelRes = select(MaxD+1, &readfds, NULL, NULL, &timeout)) < 1)
        {
            if (errno != EINTR)
            {
                printf("ошибка select: %d\n", errno);
                return(-1);
            }else
            {
                printf("пришёл неигнорируемый сигнал: %d\n", errno);
                return(-1);
            }
            continue;
        }else if (SelRes == 0)
        {
            if(bombExploding)
            {
                bombExploding = !explode(Object.GetY(), Object.GetX(), PositionBorders);
            }
        }

        //общение с клиентом:

        if(FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (10 != (key = getch()))
            {   
                switch (key)
                {
                case KEY_UP:
                    messangeFor = KEY_UP;
                    position.y--;
                    break;
                case KEY_RIGHT:
                    messangeFor = KEY_RIGHT;
                    position.x++;
                    break;
                case KEY_LEFT:
                    messangeFor = KEY_LEFT;
                    position.x--;
                    break;
                case KEY_DOWN:
                    messangeFor = KEY_DOWN;
                    position.y++;
                    break;
                default:
                    break;
                } 
                MustSend = true;
            }else return(-1);
        }

        //конец

        //начало прямого общения с сервиром:

        if(FD_ISSET(sd, &readfds))
        {
            if (0 > (ReadBytes = read(sd, &MessangeFrom, sizeof(MessangeFrom))))
            {   
                printf( "read error:%d\n", errno);
                return(-1);
            }
            else if(ReadBytes == 0)
            {
                close(sd);
                endwin();
                printf( "novogo goda ne bydet, idi nahyi\n");
                return(1);
            }
            Object = MessangeFrom;
            Object.Hide();
            if (Object.getStatue() == active) 
            {
                Object.Show();
            }else if (Object.getType() == BombType)
            {
                if (Object.getStatue() == exploded) 
                {
                    bombExploding = true;
                    if(position.x > Object.GetX() - AffectedArea*AffectedAreaXCoefficient)
                    {
                        if(position.x < Object.GetX() + AffectedArea*AffectedAreaXCoefficient)
                        {
                            if(position.y > Object.GetY() - AffectedArea*AffectedAreaYCoefficient)
                            {
                                if(position.y < Object.GetY() + AffectedArea*AffectedAreaYCoefficient)
                                {
                                    close(sd);
                                    endwin();
                                    return(1);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (MustSend)
        {
            if(write(sd, &messangeFor, sizeof(&messangeFor)) == -1)
            {
                printf("ошибка: %d", errno);
                return -1;
            }
            MustSend = false;
        } 
        //конец
        refresh();
    }
    close(sd);
    endwin();
    return 0;
}