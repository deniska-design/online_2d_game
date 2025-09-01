#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>
#include <unistd.h>
#include <ncurses.h>

#include "vector.h"
#include "player.h"
#include "typeless_type.h"

const char *ServerIp = "192.168.1.120";
int ServPort = 9;

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

int SetFdss(int fd, fd_set &readfds, fd_set &writefds, fd_set &exceptfds)
{
	FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

	FD_SET(fd, &readfds);
    FD_SET(fd, &exceptfds);
    FD_SET(fd, &writefds);
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
    Vector Position; 
    Vector PositionBorders;
    typeless messangeFor((int *)0, INT); 
    bool send = false;
    int sd, MaxD, SelRes, ReadBytes, key, messangeFrom;
    struct sockaddr_in ServAddr;
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    
    ServAddr = FillAddr(ServAddr, ServerIp, ServPort);
    if (errno == -1)
    {
        return -1;
    }
    
    if ((sd = CreateAndConnectTo(ServAddr)) == -1)
    {
        return -1;
    }

    if (0 > (ReadBytes = read(sd, &messangeFrom, sizeof(messangeFrom))))
    {   
        printf( "while connect read error:%d\n", errno);
        return(-1);
    }

    MaxD = sd;

    StartWindow();

    player Player(5, 2);

    //начало бесконечного цыкла

    while (true)
    {
        SetFdss(sd, readfds, writefds, exceptfds);
        FD_SET(STDIN_FILENO, &readfds);

        if ((SelRes = select(MaxD+1, &readfds, &writefds, &exceptfds, NULL)) < 1)
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
        }

        //общение с клиентом:

        if(FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (10 != (key = getch()))
            {   
                switch (key)
                {
                case KEY_UP:
                    messangeFor.setValue((int *)KEY_UP, INT);
                    break;
                case KEY_RIGHT:
                    messangeFor.setValue((int *)KEY_RIGHT, INT);
                    break;
                case KEY_LEFT:
                    messangeFor.setValue((int *)KEY_LEFT, INT);
                    break;
                case KEY_DOWN:
                    messangeFor.setValue((int *)KEY_DOWN, INT);
                    break;
                default:
                    break;
                } 
            }else return(-1);
        }

        //конец

        //начало прямого общения с сервиром:

        if(FD_ISSET(sd, &readfds))
        {
            if (0 > (ReadBytes = read(sd, &Position, sizeof(Position))))
            {   
                printf( "read error:%d\n", errno);
                return(-1);
            }
            else if(ReadBytes == 0)
            {
                printf( "novogo goda ne bydet, idi nahyi\n");
                return 0;
            }
            Player.setPosition(Position.y, Position.x);
            Player.hidePlayer();
            Player.showPlayer();
        }

        if(FD_ISSET(sd, &writefds))
        {
            switch (messangeFor.type)
            {
            case INT:
                if (*static_cast<int*>(messangeFor.value) != 0)
                {
                    send = true;
                }   
                break;
            case VECTOR:
                if (*static_cast<Vector*>(messangeFor.value) != Vector{0, 0})
                {
                    send = true;
                }   
                break;
            default:
                break;
            }
            if (send)
            {
                if(write(sd, &messangeFor, sizeof(messangeFor)) == -1)
                {
                    printf("ошибка: %d", errno);
                    return -1;
                }
                messangeFor.value = (int *)0;
            } 
        }

        //конец

        if(FD_ISSET(sd, &exceptfds))
        {
            printf("произошла исключительная ситуация\n");
            return -1;
        }
        refresh();
    }
    close(sd);
    endwin();
    return 0;
}
