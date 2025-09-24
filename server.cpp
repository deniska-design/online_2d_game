#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstdio>	
#include <unistd.h>
#include <stdio.h>
#include <ncurses.h>
#include <variant>

#include "vector.h"
#include "game.h"
#include "time.h"

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
	MaxPlayerCount = 4,
	MaxBombCount = 1
};

enum 
{
	MaxBombYPos	=	25,      
    MaxBombXPos	= 	40,      
    MinBombXPos	=	1,      
    MinBombYPos =	1,	  
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

int SetFdss(int sd, int playerCount, int *pd, fd_set &readfds)
{
	FD_ZERO(&readfds);
	if(playerCount < 4)
	{
		FD_SET(sd, &readfds);
	}
	for(int i = 0; i < playerCount; i++)
	{
		FD_SET(pd[i], &readfds);
	}
	return 0;
}

void PlayerLeaved(int &playerCount, int *pd, fd_set fds, int playerNum, game Game)
{
	printf("игрок ливнул. туда его\n");
	shutdown(pd[playerNum], SHUT_RDWR);
	close(pd[playerNum]);
	FD_CLR(pd[playerNum], &fds);
	Game.GetPlayer(playerNum) = Game.GetPlayer(playerCount-1);
	pd[playerNum] = pd[playerCount-1];
	playerCount--;
}

void SetMessangeForAll(object &messangeForAll, int &WhowMustSend, int NewWhomMustSend, bool mustSendAll[4], object newValue)
{
	messangeForAll = newValue;
	WhowMustSend = NewWhomMustSend;
	for (int n = 0; n < WhowMustSend; n++)	
	{
		mustSendAll[n] = true;
	}
}

void SetMessange(object messange[4], object *newValue, bool mustSendMessangeto[4], int WhowMustSend, int &messangeLenght, int newMessangeLength)
{
	for (int n = 0; n <= newMessangeLength; n++)		
	{
		messange[n] = newValue[n];
	}
	mustSendMessangeto[WhowMustSend] = true;
	messangeLenght = newMessangeLength;
}

int main()
{   
	struct timeval timeout;
	game Game(MaxPlayerCount, MaxBombCount);
    std::variant<Vector, int> messangeFrom[MaxPlayerCount];
	object messangeForAll;
	object messange[MaxPlayerCount]; 
	Vector PositionBorders[MaxPlayerCount];
	bool mustSendMessangeto[MaxPlayerCount], mustSendAll[MaxPlayerCount], positionChanged = false, BombGenerated = false;
    int sd, MaxD, ReadBytes, key, SelRes = 0, RandomTime = 0, messangeLenght = 0, WhowMustSend = 0, playerCount = 0;
    int pd[MaxPlayerCount];
	const int firstMessange = 1;
    struct sockaddr_in PlayerAddr[4];
    struct sockaddr_in ServAddr;
    fd_set readfds;

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
    socklen_t addrlen = sizeof(PlayerAddr[0]);

    keypad(stdscr, 1);

    while (true)
    {
	SetFdss(sd, playerCount, pd, readfds);

		timeout.tv_usec = 1;
        if ((SelRes = select(MaxD+1, &readfds, NULL, NULL, &timeout)) < 0)
        {
            if (errno != EINTR)
            {
                printf("ошибка select: %d\n", errno);
                return(-1);
            }else
            {
                printf("пришёл неигнорируемый сигнал: %d\n", errno);
            }
        }else if (SelRes == 0)
		{
			if(playerCount > 0)
			{
				if(!BombGenerated)
				{
					Game.GetBomb(MaxBombCount - 1).setPosition(Random(MinBombYPos, MaxBombYPos), Random(MinBombXPos, MaxBombXPos));
					Game.GetBomb(MaxBombCount - 1).setStatue(active);
					RandomTime = Random(2, 4);
					stopwatch(RandomTime, time(NULL));
					SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetBomb(MaxBombCount - 1));
					BombGenerated = true;
				}else
				{
					if(true == stopwatch(RandomTime, time(NULL)))
					{
						Game.GetBomb(MaxBombCount - 1).setStatue(disactiv);
						SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetBomb(MaxBombCount - 1));
						BombGenerated = false;
					}
				}
			}
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
			Game.GetPlayer(playerCount-1).setPosition(0, 0);
			Game.GetPlayer(playerCount-1).setStatue(alive);
			SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetPlayer(playerCount-1));
			SetMessange(messange, Game.GetPlayerArray(), mustSendMessangeto, playerCount-1, messangeLenght, playerCount-1);
		}

	//начало работы с игроками на прямую:

        for (int i = 0; i < playerCount; i++)
        {
            if(FD_ISSET(pd[i], &readfds))
            {
		    	printf("пришло сообщение от игрока\n");
				if (0 > (ReadBytes = read(pd[i], &messangeFrom[i], sizeof(&messangeFrom[i]))))	//оптимизация: пусть игрок который сделал шаг сам себя ресует и мы не будем ему отпрявлять его нувую позицию
				{																				//оптимизация: лучше не отпрявлять игроку каждый пиксель взрыва бомбы лучше просто отправить что ещё больше нет и он сам нарисует
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
							if (Game.GetPlayer(i).GetY() > 0)	
							{						
								Game.GetPlayer(i).GetY()--;	
							}
							positionChanged = true;
							break;
						case right:
							if(Game.GetPlayer(i).GetX() < PositionBorders[i].x)
							{
								Game.GetPlayer(i).GetX()++;
							}
							positionChanged = true;
							break;
						case left:
							if (Game.GetPlayer(i).GetX() > 0)
							{
								Game.GetPlayer(i).GetX()--;
							}
							positionChanged = true;
							break;
						case down:
							if(Game.GetPlayer(i).GetY() < PositionBorders[i].y)
							{
								Game.GetPlayer(i).GetY()++;
							}
							positionChanged = true;
							break;
						default:
							positionChanged = false;
							break;	
						}
						if(positionChanged)
						{
							Game.GetPlayer(i).setStatue(alive);
							SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetPlayer(i));
							printf("position changed\n");
						}
					}else if (std::holds_alternative<Vector>(messangeFrom[i]))
					{
						PositionBorders[i] = std::get<Vector>((messangeFrom[i]));
						printf("position border x:%d\n", PositionBorders[i].x);
						messangeFrom[i] = 0;
					}
				}else	
				{ 
					Game.GetPlayer(i).setStatue(dead);
					SetMessangeForAll(messangeForAll, WhowMustSend, playerCount - 1, mustSendAll, Game.GetPlayer(i));	
					PlayerLeaved(playerCount, pd, readfds, i, Game);
				}
			}
        }
		
		for (int i = 0; i < WhowMustSend; i++)  
		{
			if (mustSendAll[i])
			{
				printf("пришло время отправить сообщение игроку\n");
				if(write(pd[i], &messangeForAll, sizeof(messangeForAll)) == -1)
				{
					printf("ошибка отправки сообщения:%d", errno);
					return -1;
				}else printf("messange was sent\n");
				mustSendAll[i] = false;
			}
		}
		WhowMustSend = 0;
		for (int i = 0; i < playerCount; i++)
		{
			if (mustSendMessangeto[i])
			{
				printf("пришло время отправить длиное сообщение одному игроку\n");
				while(messangeLenght >= 0)	
				{
					if(write(pd[i], &messange[messangeLenght], sizeof(messange[messangeLenght])) == -1)
					{
						printf("ошибка отправки сообщения:%d", errno);
						return -1;
					}else printf("messange was sent\n");
					messangeLenght--;
				}
				mustSendMessangeto[i] = false;
			}
		}
	//конец
		}
    
    for (int i = 0; i < playerCount; i++)
    {
		shutdown(pd[i], SHUT_RDWR);
        close(pd[i]);
    }
    close(sd);
    return 0;
}