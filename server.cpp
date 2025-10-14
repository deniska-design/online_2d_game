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
	MaxBombCount = 1,

	MaxBombYPos	=	25,      
    MaxBombXPos	= 	80,      
    MinBombXPos	=	1,      
    MinBombYPos =	1,	

	BombExplodingTime = 1,
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

void SetMessangeForAll(object &messangeForAll, int &WhowMustSend, int NewWhomMustSend, bool mustSendAll[4], object newValue, int exception)
{
	messangeForAll = newValue;
	WhowMustSend = NewWhomMustSend;
	for (int n = 0; n < WhowMustSend; n++)	
	{
		if(n != exception)
		{
			mustSendAll[n] = true;
		}
	}	
}

template<typename parent, typename kid>
void SetMessange(parent *messange, kid *newValue, bool mustSendMessangeto[4], int WhowMustSend, int &messangeLenght, int newMessangeLength, int exception)
{
	for (int n = 0; n < newMessangeLength; n++)		
	{
		if(n != exception)
		{
			messange[n] = newValue[n];
		}
	}
	mustSendMessangeto[WhowMustSend] = true;
	messangeLenght = newMessangeLength-1;
}

template<typename parent, typename kid1, typename kid2>
object *SetGeneralObjectArray(parent *GeneralObjectArray, int GeneralObjectArraySize,kid1 *A1, int A1Size,kid2 *A2, int A2Size)
{
	int i = 0;
	while(i < GeneralObjectArraySize)
	{
		for(int n = 0; n < A1Size; n++)
		{
			if(i < GeneralObjectArraySize)
			{
				GeneralObjectArray[i] =  A1[n];
				i++;
			}
		}
		for(int n = 0; n < A2Size; n++)
		{
			if(i < GeneralObjectArraySize)
			{
				GeneralObjectArray[i] =  A2[n];
				i++;
			}
		}
	}
	return GeneralObjectArray;
}

int main()
{   
	struct timeval timeout;
	game Game(MaxPlayerCount, MaxBombCount);
    player messangeFrom[MaxPlayerCount];
	object messangeForAll;
	object messange[MaxPlayerCount+MaxBombCount], *GeneralObjectArray = new object[MaxPlayerCount+MaxBombCount]; 
	bool mustSendMessangeto[MaxPlayerCount], mustSendAll[MaxPlayerCount],
		BombGenerated = false, MustGenerateBomb = true;
    int sd, MaxD, ReadBytes, SelRes = 0, RandomTime = 0, messangeLenght = 0,
		WhowMustSend = 0, playerCount = 0, BombCount = 0;
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
    srand(time(NULL));
    keypad(stdscr, 1);

    while (true)
    {
		SetFdss(sd, playerCount, pd, readfds);
		timeout.tv_usec = 500000;
		SelRes = select(MaxD+1, &readfds, NULL, NULL, &timeout);
		for (int i = 0; i < WhowMustSend; i++)  
		{
			if (mustSendAll[i])
			{
				printf("пришло время отправить сообщение игроку\n");
				if(write(pd[i], &messangeForAll, sizeof(messangeForAll)) == -1)
				{
					printf("ошибка отправки сообщения:%d", errno);
					break;
				}else printf("X: %d, Y: %d\n", messangeForAll.GetX(), messangeForAll.GetY());
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
						break;
					}else printf("X: %d, Y: %d\n", messange[messangeLenght].GetX(), messange[messangeLenght].GetY());
					messangeLenght--;
				}
				mustSendMessangeto[i] = false;
			}
		}
		if(playerCount > 0)
		{
			printf("щас будем хуярить\n");
			if(MustGenerateBomb)
			{
				printf("создём пукалку\n");
				Game.GetBomb(MaxBombCount - 1).setPosition(Random(MinBombYPos, MaxBombYPos), Random(MinBombXPos, MaxBombXPos));
				Game.GetBomb(MaxBombCount - 1).setStatue(active);
				RandomTime = Random(2, 4);
				stopwatch(RandomTime, time(NULL), 0);
				SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetBomb(BombCount), -1);
				BombGenerated = true;
				MustGenerateBomb = false;
				BombCount++;
				continue;
			}
			if(BombGenerated)
			{
				printf("ща ещё чуть чуть\n");
				if(stopwatch(RandomTime, time(NULL), 0))
				{
					printf("бабах\n");
					Game.GetBomb(MaxBombCount - 1).setStatue(disactiv);
					SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetBomb(BombCount - 1), -1);
					BombGenerated = false;
					stopwatch(BombExplodingTime, time(NULL), BombExplodingTime);
					BombCount--;
					continue;
				}
			}
			if (!BombGenerated)
				if (!MustGenerateBomb)
					if(true == stopwatch(BombExplodingTime, time(NULL), BombExplodingTime))
					MustGenerateBomb = true;
		}
        if (SelRes < 0)
        {
            if (errno != EINTR)
            {
                printf("ошибка select: %d\n", errno);
                break;
            }else
            {
                printf("пришёл неигнорируемый сигнал: %d\n", errno);
            }
        }else if(SelRes > 0)
		{
			if(FD_ISSET(sd, &readfds))
			{
				if ((AcceptNewPlayer(sd, pd, PlayerAddr, playerCount, MaxD, addrlen)) == -1)
				{
					break;
				}
				if(write(pd[playerCount-1], &firstMessange, sizeof(firstMessange)) == -1)
				{
					printf("ошибка отправки первого сообщения:%d", errno);
					break;
				}
				Game.GetPlayer(playerCount-1).setStatue(alive);
				GeneralObjectArray = SetGeneralObjectArray<object, player, bomb>(GeneralObjectArray, playerCount+BombCount, Game.GetPlayerArray(), playerCount, Game.GetBombArray(), BombCount);	//надо сделать так что бы новому игроку приходили позиции всехз кроме него
				SetMessange<object, object>(messange, GeneralObjectArray, mustSendMessangeto, playerCount-1, messangeLenght, playerCount+BombCount, playerCount-1);
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
						break;
					}else if(ReadBytes > 0)
					{
						Game.GetPlayer(i) = messangeFrom[i];
						SetMessangeForAll(messangeForAll, WhowMustSend, playerCount, mustSendAll, Game.GetPlayer(i), i);	
						printf("position changed\n");
					}else	
					{ 
						Game.GetPlayer(i).setStatue(dead);
						SetMessangeForAll(messangeForAll, WhowMustSend, playerCount - 1, mustSendAll, Game.GetPlayer(i), -1);	
						PlayerLeaved(playerCount, pd, readfds, i, Game);
					}
				}
			}
		}else
		{
			printf("timeout\n");
		}
	}
    //конец
    for (int i = 0; i < playerCount; i++)
    {
		shutdown(pd[i], SHUT_RDWR);
        close(pd[i]);
    }
    close(sd);
    return -1;
}