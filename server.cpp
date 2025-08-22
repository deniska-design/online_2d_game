#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>

const char *ip = "192.168.1.120";
int ServPort = 8;

struct sockaddr_in FillServAddr(struct sockaddr_in ServAddr, const char *ip, int ServPort)
{
	ServAddr.sin_family = AF_INET;
   	ServAddr.sin_port = htons(ServPort);
    if (!inet_aton(ip, &(ServAddr.sin_addr)))
	{
		printf("error with filling sin_addr: %d\n", errno);
		return ServAddr;
	}else printf("sin_addr was filled\n");
	return ServAddr;
}

int SetSock(struct sockaddr_in ServAddr)
{
	int sd;
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("error with creating a socket: %d\n", errno);
		return -1;
	}else printf("socket was created\n");
	if (bind(sd, (struct sockaddr *)&ServAddr, sizeof(ServAddr)) == -1)
	{
		printf("error with binding socket and address: %d\n", errno);
		return -1;
	}else printf("socket was binded with addres\n");
	if (-1 == (listen(sd, 5))){
		printf("ошибка задания параметров сокета: %d\n", errno);
		return(-1);
	}else printf("socket is listening\n");
	return sd;
}

int AcceptNewPlayer(int sd, int *pd, struct sockaddr_in *PlayerAddr, int &playerCount, int &MaxD, socklen_t addrlen)
{
	printf("пришёл запрос на соедение\n");
	if (-1 == (pd[playerCount] = accept(sd, (struct sockaddr *)&PlayerAddr[playerCount], &addrlen)))
	{
		printf("ошибка принятия игрока:%d\n", errno);
		return(-1);
	}else printf("игрок принят\n");
	if (pd[playerCount] > MaxD)
	{
		MaxD = pd[playerCount];
	}
	playerCount++;
	return 0;
}

int SetFdss(int sd, int playerCount, int *pd, fd_set &readfds, fd_set &writefds, fd_set &exceptfds)
{
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);
	if(playerCount < 4)
	{
		FD_SET(sd, &readfds);
		FD_SET(sd, &writefds);
		FD_SET(sd, &exceptfds);
	}
	for(int i = 0; i < playerCount; i++)
	{
		FD_SET(pd[i], &readfds);
		FD_SET(pd[i], &writefds);
		FD_SET(pd[i], &exceptfds);
	}
	return 0;
}

int PlayerLeaved(int &playerCount, int *pd, fd_set fds, int playerNum)
{
	printf("игрок ливнул. туда его\n");
	shutdown(pd[playerNum], SHUT_RDWR);
	close(pd[playerNum]);
	FD_CLR(pd[playerNum], &fds);
	pd[playerNum] = pd[playerCount-1];
	playerCount--;
	return 0;
}

int SendMessange(int recipSock, int &messange)
{
	printf("пришло время отправить сообщение игроку\n");
	if(write(recipSock, &messange, sizeof(messange)) == -1)
	{
		printf("ошибка отправки сообщения:%d", errno);
	}else printf("messange was sent\n");
	messange = 0;
	return 0;
}

int main()
{   
    int messangeFrom[4];
    int messangeFor[4];
    int sd, MaxD, SelRes, ReadBytes;
    int playerCount = 0;
    int pd[4];
    struct sockaddr_in PlayerAddr[4];
    struct sockaddr_in ServAddr;
    fd_set readfds, writefds, exceptfds;

    ServAddr = FillServAddr(ServAddr, ip, ServPort);

    sd = SetSock(ServAddr);

    MaxD = sd;
    playerCount = 0;
    socklen_t addrlen = sizeof(PlayerAddr[0]);

    keypad(stdscr, 1);

    while (true)
    {
		
	SetFdss(sd, playerCount, pd, readfds, writefds, exceptfds);

        if ((SelRes = select(MaxD+1, &readfds, NULL, &exceptfds, NULL)) == -1)
        {
            if (errno != EINTR)
            {
                printf("ошибка select: %d\n", errno);
                return(-1);
            }else
            {
                printf("пришёл неигнорируемый сигнал: %d\n", errno);
            }
            continue;
        }

		if(FD_ISSET(sd, &readfds))
		{
			AcceptNewPlayer(sd, pd, PlayerAddr, playerCount, MaxD, addrlen);
		}

        if (FD_ISSET(sd, &exceptfds))
        {
            printf("произошла исключительная ситуация на сокете сервера\n");
        }
	
	//начало работы с игроками на прямую:

        for (int i = 0; i < playerCount; i++)
        {
            if(FD_ISSET(pd[i], &readfds))
            {
		    	printf("пришло сообщение от игрока\n");
				if (0 > (ReadBytes = read(pd[i], &messangeFrom[i], sizeof(messangeFrom[i]))))
				{
					printf("ошибка чтения данных:%d\n", errno);
					return(-1);
				}else if(ReadBytes > 0)
				{
					printf("сообщение от игрока: %d\n", messangeFrom[i]);
					switch (messangeFrom[i])
					{
					case KEY_UP:
						messangeFor[i] = KEY_UP;
						printf("сообщение для игрока:%d", messangeFor[i]);
						break;
					case KEY_RIGHT:
						messangeFor[i] = KEY_RIGHT;
						printf("сообщение для игрока:%d", messangeFor[i]);
						break;
					case KEY_LEFT:
						messangeFor[i] = KEY_LEFT;
						printf("сообщение для игрока:%d", messangeFor[i]);
						break;
					case KEY_DOWN:
						messangeFor[i] = KEY_DOWN;
						printf("сообщение для игрока:%d", messangeFor[i]);
						break;
					default:
						break;
					}
				}else PlayerLeaved(playerCount, pd, readfds, i);
			}
        }

        for (int i = 0; i < playerCount; i++)  
        {
            if (messangeFor[i] != 0)
            {
                if(FD_ISSET(pd[i], &writefds))
                {
					SendMessange(pd[i], messangeFor[i]);
                }
            }
        }

	//конец

        for (int i = 0; i < playerCount; i++)
        {
            if(FD_ISSET(pd[i], &exceptfds))
            {
                printf("произошла исключительная ситуация на сокете игрока\n");
            }  
        }
    }
    for (int i = 0; i < playerCount; i++)
    {
		shutdown(pd[i], SHUT_RDWR);
        close(pd[i]);
    }
    close(sd);
    return 0;
}
