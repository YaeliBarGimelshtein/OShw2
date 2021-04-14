//208730523 (Yarden Dahan), 208722710 (Yaeli Gimelshtein)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Question_Solution.h"

#define PROG_PIPES 1
#define PROG_MMAP 2
#define PROG_THREADS 3
#define PROG_FORK 4
#define PROG_SEMAPHORE 5
#define EXIT 6

void menu_prompt(int *choice)
{
	printf("HELLO-I'M A BUNNY\n   (\\ /)\n   (•.•)\n   (\")(\")\n");
	printf("For program 1 (pipes) - press %d\n",PROG_PIPES);
	printf("For program 2 (cyclic buffer and shared memory) - press %d\n",PROG_MMAP);
	printf("For program 3 (threads) - press %d\n",PROG_THREADS);
	printf("For program 4 (processes) - press %d\n",PROG_FORK);
	printf("For program 5 (semaphore) - press %d\n",PROG_SEMAPHORE);
	printf("Exit program - press %d\n", EXIT);
	scanf("%d", choice);
}

int main()
{
	int size;
	//opening the file
	FILE* myfile = fopen("Numbers_Array.txt","r");
	if(!myfile)
	{
		printf("open file filed\n");
		exit(EXIT_FAILURE);
	}
	//getting array size
	if(fscanf(myfile,"%d",&size)==0)
	{
		printf("You don't have nubers in file- bye bye\n");
		exit(EXIT_SUCCESS);
	}
	//getting all numbers to the array
	int* nums = (int*)malloc(sizeof(int)*size);
	if(!nums)
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < size; i++)
	{
		if(fscanf(myfile,"%d",&nums[i])==0)
		{
			printf("read number failed bye-bye\n");
			exit(EXIT_SUCCESS);
		}
	}
	//initialzing the limit to all questions
	init_limit(size);

	//menu
	int choice = 0;
	do {
		menu_prompt(&choice);
		switch (choice) {
		case PROG_PIPES:
			printf("Now we will run processes that communicate through pipes\n");
			doF321Question1(nums);
			break;
		case PROG_MMAP:
			printf("Now we will run processes that shared memory (mmap) and communicate through ring buffer\n");
			doF321Question2(nums);
			break;
		case PROG_THREADS:
			printf("Now we will run for each value a separate thread\n");
			doF321Question3(nums);
			break;
		case PROG_FORK:
			printf("Now we will run for each value a separate process\n");
			doF321Question4(nums);
			break;
		case PROG_SEMAPHORE:
			printf("Now we will run processes that communicate through semaphore\n");
			doF321Question5(nums);
			break;
		case EXIT:
			printf("Bye-Bye\n");
			break;
		default:
			printf("Invalid choice- Try again\n");
		}
	}
	while (choice != EXIT);
}
