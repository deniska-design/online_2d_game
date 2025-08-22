#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <ncurses.h>

#include "player.h"

const char *ServerIp = "192.168.1.120";
int ServPort = 8;

enum {secret_code = 2364};

struct sockaddr_in FillServAddr(struct sockaddr_in ServAddr, const char *ip, int ServPort)
{
	ServAddr.sin_family = AF_INET;
   	ServAddr.sin_port = htons(ServPort);
    if (!inet_aton(ip, &(ServAddr.sin_addr)))
	{
        printf("ошибка: %d", errno);
		return ServAddr;
	}
	return ServAddr;
}

int CreateAndConnectTo(struct sockaddr_in ServAddr)
{
    int messangeFrom;
	int sd = 0;
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
        printf("ошибка: %d", errno);
		return -1;
	}

    do 
    {
        printf("подключение к серверу\n");
        if (-1 == (connect(sd, (struct sockaddr *)&ServAddr, sizeof(ServAddr))))
        {   
            printf("ошибка: %d", errno);
            return -1;
        }
    read(sd, &messangeFrom, sizeof(messangeFrom));
    }while (messangeFrom != secret_code);
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

int SendMessange(int recipSock, int &messange)
{
	if(write(recipSock, &messange, sizeof(messange)) == -1)
	{
		printf("ошибка: %d", errno);
        return -1;
	}
	messange = 0;
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
    int messangeFrom;
    int messangeFor = 0;
    int sd, MaxD, SelRes, ReadBytes, key;
    struct sockaddr_in ServAddr;
    fd_set readfds, writefds, exceptfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    
    ServAddr = FillServAddr(ServAddr, ServerIp, ServPort);
    
    sd = CreateAndConnectTo(ServAddr);

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
                    messangeFor = KEY_UP;
                    mvprintw(0, 0, "messange for:%d", messangeFor);
                    break;
                case KEY_RIGHT:
                    messangeFor = KEY_RIGHT;
                    mvprintw(0, 0, "messange for:%d", messangeFor);
                    break;
                case KEY_LEFT:
                    messangeFor = KEY_LEFT;
                    mvprintw(0, 0, "messange for:%d", messangeFor);
                    break;
                case KEY_DOWN:
                    messangeFor = KEY_DOWN;
                    mvprintw(0, 0, "messange for:%d", messangeFor);
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
            if (0 > (ReadBytes = read(sd, &messangeFrom, sizeof(messangeFrom))))
            {   
                printf( "read error:%d\n", errno);
                return(-1);
            }
            else if(ReadBytes == 0)
            {
                printf( "novogo goda ne bydet, idi nahyi\n");
                return 0;
            }
            mvprintw(1, 0, "messsange from: %d\n", messangeFrom);
        }

        if (messangeFor != 0)
        {
            if(FD_ISSET(sd, &writefds))
            {
                SendMessange(sd, messangeFor);
            }
        }
        
        //конец

        if(FD_ISSET(sd, &exceptfds))
        {
            printf("произошла исключительная ситуация\n");
            return -1;
        }
        Player.showPlayer();
        refresh();
    }
    close(sd);
    endwin();
    return 0;
}
