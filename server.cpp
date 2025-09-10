#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>	//ehdgkjdf
#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>
#include <variant>

#include "vector.h"

const char *ip = "192.168.1.120";
int ServPort = 10;

typedef enum 
{
	right	=	261,      // Key: Cursor right
    left	=	260,      // Key: Cursor left
    down	= 	258,      // Key: Cursor down
    up		=	259,	  // Key: Cursor up
}keybordArrows;

enum 
{
	ClrScreen	=	1,      // Key: Cursor right
    nClrScreen	=	0,      // Key: Cursor left
};

struct sockaddr_in FillAddr(struct sockaddr_in ServAddr, const char *ip, int ServPort)
{
	ServAddr.sin_family = AF_INET;
   	ServAddr.sin_port = htons(ServPort);
    if (!inet_aton(ip, &(ServAddr.sin_addr)))
	{
		printf("error with filling sin_addr: %d\n", errno);
		errno = -1;
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

int main()
{   
    std::variant<Vector, int> messangeFrom[4];
	Vector messange[4]; 
	Vector messangeForAll;
	Vector position[4];
	Vector PositionBorders[4];
	bool mustSendMessangeto[4];
	bool mustSendAll[4];
    int sd, MaxD, ReadBytes, key, messangeLenght;
    int playerCount = 0;
    int pd[4];
	const int firstMessange = 1;
    struct sockaddr_in PlayerAddr[4];
    struct sockaddr_in ServAddr;
    fd_set readfds, writefds, exceptfds;

    ServAddr = FillAddr(ServAddr, ip, ServPort);
	if (errno == -1)
	{
		return -1;
	}
	
    if((sd = SetSock(ServAddr)) == -1)
	{
		return -1;
	}

    MaxD = sd;
    playerCount = 0;
    socklen_t addrlen = sizeof(PlayerAddr[0]);

    keypad(stdscr, 1);

    while (true)
    {
		
	SetFdss(sd, playerCount, pd, readfds, writefds, exceptfds);

        if ((select(MaxD+1, &readfds, &writefds, &exceptfds, NULL)) == -1)
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
			if ((AcceptNewPlayer(sd, pd, PlayerAddr, playerCount, MaxD, addrlen)) == -1)
			{
				return -1;
			}
			if(write(pd[playerCount-1], &firstMessange, sizeof(firstMessange)) == -1)
			{
				printf("ошибка отправки первого сообщения:%d", errno);
				return -1;
			}
			messangeForAll = position[playerCount-1];
			for (int n = 0; n < playerCount; n++)		//можно написать функцию которя будет инициализировать сообщение
			{
				mustSendAll[n] = true;
				messange[n] = position[n];
			}
			mustSendMessangeto[playerCount-1] = true;
			messangeLenght = playerCount;
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
				if (0 > (ReadBytes = read(pd[i], &messangeFrom[i], sizeof(&messangeFrom[i]))))
				{
					printf("ошибка чтения данных:%d\n", errno);
					return -1;
				}else if(ReadBytes > 0)
				{
					if(std::holds_alternative<int>(messangeFrom[i]))
					{
						key = std::get<int>(std::move(messangeFrom[i]));
						printf("key:%d\n", key);
						switch (key)
						{
						case up:
							if (position[i].y > 0)	//зачем серверу получать границы и потом проверять столкновение с ними
							{						//пусть игрок сам проверяет это и если игрок заходит за границы не отправляет серверу сообщение
								position[i].y--;	
							}
							break;
						case right:
							if(position[i].x < PositionBorders[i].x)
							{
								position[i].x++;
							}
							break;
						case left:
							if (position[i].x > 0)
							{
								position[i].x--;
							}
							break;
						case down:
							if(position[i].y < PositionBorders[i].y)
							{
								position[i].y++;
							}
							break;
						default:
							break;
						}
						for (int n = 0; n < playerCount; n++)
						{
							messangeForAll = position[i];
							mustSendAll[n] = true;
						}
						printf("position changed\n");
					}else if (std::holds_alternative<Vector>(messangeFrom[i]))
					{
						PositionBorders[i] = std::get<Vector>(std::move(messangeFrom[i]));
						printf("position border x:%d\n", PositionBorders[i].x);
						messangeFrom[i] = 0;
					}
				}else PlayerLeaved(playerCount, pd, readfds, i);
			}
        }
		
		for (int i = 0; i < playerCount; i++)  
		{
			if (mustSendAll[i])
			{
				if(FD_ISSET(pd[i], &writefds))
				{
					printf("пришло время отправить сообщение игроку\n");
					if(write(pd[i], &messangeForAll, sizeof(&messangeForAll)) == -1)
					{
						printf("ошибка отправки сообщения:%d", errno);
						return -1;
					}else printf("messange was sent\n");
					mustSendAll[i] = false;
				}
			}
			if (mustSendMessangeto[i])
			{
				if(FD_ISSET(pd[i], &writefds))
				{
					for(;messangeLenght > 0; messangeLenght--)	//можно сделать переменую в которой будет записано сколько надо отправить
					{
						printf("пришло время отправить сообщение игроку\n");
						if(write(pd[i], &messange[messangeLenght], sizeof(&messange[messangeLenght])) == -1)
						{
							printf("ошибка отправки сообщения:%d", errno);
							return -1;
						}else printf("messange was sent\n");
						messange[messangeLenght] = (Vector){0, 0};
					}
					mustSendMessangeto[i] = false;
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