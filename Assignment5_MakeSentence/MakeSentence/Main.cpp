#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<assert.h>
#include<crtdbg.h>

int main()
{
	//_CrtSetBreakAlloc();
	{
		//variables
		char * sentence = (char*)malloc(sizeof(char));
		assert(sentence);
		size_t* sentence_size = (size_t*)malloc(sizeof(size_t));
		assert(sentence_size);
		//char* word = (char*)malloc(sizeof(char));
		size_t* word_size = (size_t*)malloc(sizeof(size_t));
		assert(word_size);
		char* c = (char*)malloc(sizeof(char));
		assert(c);
		char* temp;

		sentence[0] = '\0';
		*sentence_size = 1;


		printf("Input word. Press enter for new word. Press ENTER to end input and make sentence.\n");

		while (1)
		{
			//input character
			scanf_s("%c", &(*c), 1);

			//if enter pressed
			//check if word entered or not
			if (*c == '\n')
			{
				//if word not entered; break
				if (*word_size == 0)
				{
					sentence[(*sentence_size) - 2] = '.';
					sentence[(*sentence_size) - 1] = '\0';
					break;
				}
				else
				{
					//add space after word as word size is greater than 0
					temp = (char*)realloc(sentence, ((*sentence_size) + 1) * sizeof(char));
					assert(temp);
					if (temp != NULL)
					{
						sentence = temp;
					}
					if (sentence != NULL)
					{
						sentence[(*sentence_size) - 1] = ' ';
						sentence[(*sentence_size)] = '\0';
						(*sentence_size)++;
						(*word_size) = 0;
					}
				}
			}
			else
			{
				//add character to the sentence directly
				temp = (char*)realloc(sentence, ((*sentence_size) + 1) * sizeof(char));
				assert(temp);
				if (temp != NULL)
				{
					sentence = temp;
				}
				if (sentence != NULL)
				{
					sentence[(*sentence_size) - 1] = *c;
					sentence[(*sentence_size)] = '\0';
					(*sentence_size)++;
					(*word_size)++;
				}
			}
		}

		for (size_t i = 0; i < (*sentence_size); i++)
		{
			printf("%c", sentence[i]);
		}

		free(c);
		free(word_size);
		free(sentence_size);
		free(sentence);
	}

	_CrtDumpMemoryLeaks();
	return 0;
}