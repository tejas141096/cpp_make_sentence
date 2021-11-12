#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "Math/Point2D.h"
#include <Point2D.h>

//Player class
class Player
{
private:
	char* playerName = nullptr;
	int playerX = 0;
	int playerY = 0;
	//Point2D playerCoordinate;

public:
	Player() {}
	~Player() {}

	void setPlayer(char* s)
	{
		playerName = s;
	}

	void printPlayer()
	{
		printf("Player - ");
		for (size_t i = 0; i < strlen(playerName); i++)
		{
			printf("%c", playerName[i]);
		}
		printf(": [ %d , %d ]\n", playerX, playerY);
		//printf("%d", strlen(name));
	}

	void playerMovement(char direction)
	{
		if (direction == 'w')
			playerY += 1;
		if (direction == 's')
			playerY -= 1;
		if (direction == 'a')
			playerX -= 1;
		if (direction == 'd')
			playerX += 1;
	}
};

//Monster class
class Monster
{
private:
	char* monsterName = nullptr;
	int monsterX = 0;
	int monsterY = 0;

public:
	int monsterLife = 0;
	Monster() {}
	~Monster() {}

	bool setMonster(char* s)
	{
		monsterName = s;
		monsterX = (rand() % 101) - 50;
		monsterY = (rand() % 101) - 50;
		monsterLife = rand() % 11;
		if (monsterLife == 0)
			return true;
		else
			return false;
	}

	void printMonster()
	{
		printf("Monster - ");
		for (size_t i = 0; i < strlen(monsterName); i++)
		{
			printf("%c", monsterName[i]);
		}
		printf(": [ %d , %d ]\n", monsterX, monsterY);
		//printf("%d", monsterLife);
		//printf("%d", strlen(name));
	}

	void monsterMovement()
	{
		monsterX += (rand() % 10) - 5;
		monsterY += (rand() % 10) - 5;
	}
};

int main()
{
	Engine::Point2DUnitTest();
	
	//Setup Player
	Player* player = new Player();
	{
		char* playerName = (char*)malloc(sizeof(char));
		char* c = new char();
		size_t* nameSize = new size_t;
		*nameSize = 1;
		printf("Enter player name: ");
		while (*c != '\n')
		{
			scanf_s("%c", &(*c), 1);
			if (*c == '\n')
				break;
			playerName = (char*)realloc(playerName, (*nameSize) + 1);
			if (playerName != NULL)
			{
				playerName[(*nameSize) - 1] = *c;
				playerName[(*nameSize)] = '\0';
				(*nameSize)++;
			}

		}

		player->setPlayer(playerName);
		//player->printPlayer();
	}


	//Setup Monsters
	unsigned int* numberOfMonsters = new unsigned int();
	printf("Enter number of monsters: ");
	scanf_s("%d", &(*numberOfMonsters));
	printf("Number of monsters: %d\n", *numberOfMonsters);
	Monster* monsters = new Monster[*numberOfMonsters]();
	unsigned int* countToDeleteMonsters = new unsigned int();
	(void)getchar();
	for (size_t i = 0; i < *numberOfMonsters; i++)
	{
		char* monsterName = (char*)malloc(sizeof(char));
		char* c = new char();
		size_t* nameSize = new size_t;
		*nameSize = 1;
		printf("Enter monster %zu name: ", i + 1);

		while (*c != '\n')
		{
			scanf_s("%c", &(*c), 1);
			if (*c == '\n')
				break;
			monsterName = (char*)realloc(monsterName, (*nameSize) + 1);
			if (monsterName != NULL)
			{
				monsterName[(*nameSize) - 1] = *c;
				monsterName[(*nameSize)] = '\0';
				(*nameSize)++;
			}
		}

		if (monsters[i].setMonster(monsterName))
		{
			(*countToDeleteMonsters)++;
		}
		//monsters[i].printMonster();
	}


	//Game Logic - Player Movement and Monster AI
	int* countDeleteMonsters = new int();

	{
		//Generate new monster every 5 turns
		unsigned int loopNumber = 1;
		//player movement input
		char movement;
		//Count of number of new monsters added
		int countAddMonster = 0;
		do
		{
			//player movement and print player and monster position
			printf("Enter 'wasd' for movement direction. Enter 'q' to quit.\n");
			movement = _getch();
			//printf("%c\n", movement);
			if (movement == 'q')
			{
				break;
			}
			player->playerMovement(movement);
			player->printPlayer();
			for (size_t i = 0; i < *numberOfMonsters; i++)
			{
				monsters[i].monsterMovement();
				monsters[i].printMonster();
			}


			/*
			* Monster addtion and deletion AI
			*/

			//check if its 5th turn in order to add memory for new monster
			int addLoop = 0;
			if (loopNumber % 5 == 0)
				addLoop++;
			//temporary monster list for storing undeleted items
			Monster* tempMonsters = new Monster[static_cast<__int64>(*numberOfMonsters) - (*countToDeleteMonsters) + (addLoop)];
			//count of monsters to be deleted next turn
			*countToDeleteMonsters = 0;
			//count of monsters deletd this turn
			*countDeleteMonsters = 0;
			//Monster deletion AI
			//delete monster if life is 0 and count number of monsters to delete next turn
			for (size_t i = 0, j = 0; i < *numberOfMonsters; i++)
			{
				if (monsters[i].monsterLife > 0)
				{
					--(monsters[i].monsterLife);
					if (monsters[i].monsterLife == 0)
						(*countToDeleteMonsters)++;
					tempMonsters[j++] = monsters[i];
				}
				else
				{
					(*countDeleteMonsters)++;
				}
			}
			for (unsigned int i = 0; i < static_cast<__int64>(*numberOfMonsters) - (*countDeleteMonsters); i++)
			{
				monsters[i] = tempMonsters[i];
			}
			(*numberOfMonsters) -= (*countDeleteMonsters);
			delete[] tempMonsters;
			//printf("%d", *numberOfMonsters);

			//Monster addition A
			if (loopNumber % 5 == 0)
			{
				char newMonsterName[10] = "NewB";
				sprintf_s(newMonsterName, "NewB%d", countAddMonster++);
				//printf("%s", newMonsterName);
				monsters[(*numberOfMonsters)].setMonster(newMonsterName);
				(*numberOfMonsters)++;
			}
			loopNumber++;

		} while (movement != 'q');
	}



	printf("Final Position: \n");
	player->printPlayer();
	for (size_t i = 0; i < *numberOfMonsters; i++)
	{
		monsters[i].printMonster();
	}

	delete countDeleteMonsters;
	delete countToDeleteMonsters;
	delete[] monsters;
	delete numberOfMonsters;
	delete player;

	return 0;
}
