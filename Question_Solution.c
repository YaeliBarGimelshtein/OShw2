#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include "Question_Solution.h"


//globals for all questions
int total_numbers;
sem_t mutex;


//initialzing the limit to all questions
void init_limit(int num)
{
	total_numbers = num;
}


void doF321_Semaphore(int** values) //values[0] = index, values[1] = number,values[2] = return values
{
	int* returns_values = values[2];
	int* index = values[0];
	int n1 = *values[1];
	int n = *values[1];
	int k = 0;

	while (n != 1)
	{
		if (n % 2 == 0)
			n = n/2;
		else
			n = n*3 +1;

		k = k + 1;
	}

	sem_wait(&mutex); //waiting to get into critical section
	//critical section
	printf("Thread for n1 = %d, Number of Iterations: %d\n",n1 ,k);
	returns_values[*index] = k;
	*index = *index +1;
	sem_post(&mutex); //exiting critical section and realsing the semaphore
}

int doF321_Thread(int* num)
{
	int n1 = *num;
    int n = *num;
    int k = 0;

    while (n != 1)
	{
		if (n % 2 == 0)
			n = n/2;
		else
			n = n*3 +1;

		k = k + 1;
	}
    printf("Thread for n1 = %d, Number of Iterations: %d\n",n1 ,k);
    return k;
}
///////////////**********************START QUESTION 1 *********************///////////////////////////////////////////

void doF321Question1(int nums[])
{
	int** values = (int**)malloc(2*sizeof(int*)); //values[0] = number, values[1] = total iterations
	if(!values)
	{
		printf("malloc values failed\n");
		exit(EXIT_FAILURE);
	}

	//allocating both arrays
	values[0] = (int*)malloc(sizeof(int));
	if(!values[0])
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	*values[0] = 0;
	values[1] = (int*)malloc(sizeof(int));
	if(!values[1])
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}

	//helpers
	int index_values = 0;
	int index = 0;
	int IN = 0;

	int parameters[3];  //parameters[0]=n1, parameters[1]=n, parameters[2]=k

	//creation of pipe
	int parentPipe[2]; //this is the pipe that parent reads from
	if (pipe(parentPipe) == -1)    //check success of pipe()
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	int childPipe[2]; //this is the pipe that child reads from
	if (pipe(childPipe) == -1) //check success of pipe()
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	//creation of processes
	pid_t proc = fork();
	if (proc == -1)      //check success of fork()
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	//initialazing parameters
	parameters[0] = nums[index];
	parameters[1] = nums[index];
	parameters[2] = 0;


	//processes:
	if (proc == 0)   //child
	{
		do {
			close(parentPipe[0]); //reads from own pipe so no need to read from this one
			close(childPipe[1]); //writes to other pipe so no need to write to this one
			read(childPipe[0], parameters, sizeof(int) * 3);

			if (parameters[1] == 0) //no more numbers to work on
			{
				close(parentPipe[1]);
				close(childPipe[0]);
				break;
			}

			while (parameters[1] % 2 == 0)
			{
				printf("****CHILD PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
				parameters[1] = parameters[1] / 2;
				parameters[2] = parameters[2] + 1;
			}
			write(parentPipe[1], parameters, sizeof(int) * 3);
		}
		while (1);

	}
	else //parent
	{
		do
		{
			if (index == total_numbers && IN == 0) //finished taking care of all numbers
			{
				parameters[1] = 0;
				parameters[2] = 0;
				parameters[0] = 0;
				printf("Summary:\n");
				for(int i = 0; i < total_numbers ; i++)
					printf("Number = %d --> total iterations = %d \n",values[0][i],values[1][i]);

				write(childPipe[1], parameters, sizeof(int) * 3);
				wait(NULL);
				close(parentPipe[0]);
				close(childPipe[1]);
				break;
			}
			else if (IN < total_numbers && index != total_numbers) //getting the next number if there is a place in the pipe
			{
				parameters[0] = nums[index];
				parameters[1] = nums[index];
				parameters[2] = 0;
				IN = IN + 1;
				index = index + 1;
				if (parameters[1] % 2 != 0)
				{
					parameters[1] = parameters[1] * 3 + 1;
					parameters[2] = parameters[2] + 1;
					printf("****PARENT PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
					sleep(1);
				}
				write(childPipe[1], parameters, sizeof(int) * 3);
			}
			else //no more place in the father pipe
			{
				read(parentPipe[0], parameters, sizeof(int) * 3);
				if (parameters[1] == 1)
				{
					IN = IN - 1;
					printf("****PARENT PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
					values[0][index_values] = parameters[0];
					values[1][index_values] = parameters[2];
					index_values = index_values+1;
				}
				else
				{
					if (parameters[1] % 2 != 0)
					{
						parameters[1] = parameters[1] * 3 + 1;
						parameters[2] = parameters[2] + 1;
						printf("****PARENT PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
						sleep(1);
					}
					write(childPipe[1], parameters, sizeof(int) * 3);
				}
			}
		}
		while (1);
	}
}
////////////////********************* END QUESTION 1***********************///////////////////////////////////////////
///////////////**********************START QUESTION 2 *********************///////////////////////////////////////////

void doF321Question2(int nums[])
{
	int index = 0;
	//values[0] = number, values[1] = total iterations
	int** values = (int**)mmap(NULL,2*sizeof(int*),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(values == MAP_FAILED)
	{
		printf("Mapping values failed\n");
		exit(EXIT_FAILURE);
	}
	values[0] = (int*)mmap(NULL,sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(values[0] == MAP_FAILED)
	{
		printf("map failed\n");
		exit(EXIT_FAILURE);
	}
	*values[0] = 0;
	values[1] = (int*)mmap(NULL,sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(values[1]== MAP_FAILED)
	{
		printf("map failed\n");
		exit(EXIT_FAILURE);
	}

	//inOuts[0] = in_Parent,inOuts[1] = out_Parent,inOuts[2] = in_child,inOuts[3]=out_child
	int *inOuts = (int*)mmap(NULL,4*sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(inOuts==MAP_FAILED)
	{
		printf("Mapping inOuts failed!\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < 4; i++)
		inOuts[i]=0;

	//buffers
	int **parentBuffer = (int**)mmap(NULL,BUF_SIZE*sizeof(int*),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(parentBuffer==MAP_FAILED)
	{
		printf("Mapping parentBuffer failed!\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < BUF_SIZE; i++)
		parentBuffer[i] = (int*)mmap(NULL,PARAMETERS_SIZE*sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);

	int **childBuffer = (int**)mmap(NULL,BUF_SIZE*sizeof(int*),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(childBuffer==MAP_FAILED){
		printf("Mapping childBuffer failed!\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < BUF_SIZE; i++)
		childBuffer[i] = (int*)mmap(NULL,PARAMETERS_SIZE*sizeof(int),PROT_WRITE|PROT_READ,MAP_SHARED|MAP_ANONYMOUS,0,0);

	//parameters[0]=n1,parameters[1]=n,parameters[2]=k
	int parameters[PARAMETERS_SIZE];

	//create process
	pid_t proc = fork();
	if(proc == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}

	//initialazing parameters
	parameters[0] = nums[index];
	parameters[1] = nums[index];
	parameters[2] = 0;
	int done = 0;

	if(proc == 0)  //child process
	{
		while(1)
		{
			while(inOuts[2] == inOuts[3]);//happens when buf_child empty

			for(int i = 0; i< PARAMETERS_SIZE ; i++)//reading the buffer
				parameters[i] = childBuffer[inOuts[3]][i];
			inOuts[3] = (inOuts[3]+1)%BUF_SIZE;//changing read position

			if(parameters[1] == 0)//no more numbers to work on
			{
				int err = munmap(childBuffer,BUF_SIZE*sizeof(int*));
				if(err != 0)//check unMap success
				{
					printf("unMapping child buffer failed\n");
					exit(EXIT_FAILURE);
				}

				printf("Summary:\n");
				for(int i = 0; i < total_numbers ; i++)
				{
					printf("Number = %d --> total iterations = %d \n",values[0][i],values[1][i]);

				}
				return;
			}

			while(parameters[1] %2 == 0)
			{
				parameters[1] = parameters[1]/2;
				parameters[2] = parameters[2] + 1;

				if(parameters[1]!= 1)
					printf("****CHILD PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);

			}
			while(((inOuts[0] + 1)%BUF_SIZE)== inOuts[1]); //happens when buffer papa full (no place to write)
			for(int i = 0; i < PARAMETERS_SIZE; i++)
				parentBuffer[inOuts[0]][i] = parameters[i]; //writing to parent
			inOuts[0] = (inOuts[0]+1) %BUF_SIZE;
		}
	}
	else    //parent
	{
		while(1)
		{
			if(((inOuts[0] + 1)!= inOuts[1]) && (index != total_numbers)) // getting the next number
			{
				parameters[0] = nums[index];
				parameters[1] = nums[index];
				parameters[2] = 0;
				if(parameters[1]%2 != 0)
				{
					parameters[1] = parameters[1] * 3 + 1;
					parameters[2] = parameters[2] + 1;
					printf("****PARENT PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
					sleep(1);
				}

				while(((inOuts[2]+1)%BUF_SIZE)==inOuts[3]); //Happens when buffer is full
				for(int i = 0; i < PARAMETERS_SIZE ; i++)
					childBuffer[inOuts[2]][i] = parameters[i]; //writing to child
				inOuts[2] = (inOuts[2]+1)%BUF_SIZE;
				index = index + 1;
			}
			else if(done == total_numbers && (inOuts[0]==inOuts[1])&&(inOuts[2]==inOuts[3])) //when the buffers empty and we finished all numbers
			{
				parameters[0]=0;
				parameters[1]=0;
				parameters[2]=0;

				while((inOuts[2]+1)%BUF_SIZE == inOuts[3]); //happans when buffer is full

				for(int i = 0; i < PARAMETERS_SIZE ; i++)
					childBuffer[inOuts[2]][i] = parameters[i]; //writing to child
				inOuts[2] = (inOuts[2]+1)%BUF_SIZE;
				wait(NULL);
				int err = munmap(parentBuffer,BUF_SIZE*sizeof(int*));
				if(err != 0)//check unMap success
				{
					printf("unMapping parent buffer failed\n");
					exit(EXIT_FAILURE);
				}
				return;
			}
			else // no more place in the parent buffer
			{
				while(inOuts[0]==inOuts[1]); // happans when buffer empty

				for(int i = 0; i < PARAMETERS_SIZE ; i++)
					 parameters[i] = parentBuffer[inOuts[1]][i] ; //reading the buffer
				inOuts[1] = (inOuts[1] + 1)% BUF_SIZE;
				if(parameters[1] == 1) //finished this number
				{
					values[0][done] =parameters[0];
					values[1][done] = parameters[2];
					printf("****PARENT PROCESS****\nFINAL Iteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
					done = done+1;
				}
				else //not finished
				{
					if(parameters[1]%2 != 0)
					{
						parameters[1] = parameters[1] * 3 + 1;
						parameters[2] = parameters[2] + 1;
						printf("****PARENT PROCESS****\nIteration:%d\nn1 = %d\nn = %d\n",parameters[2],parameters[0],parameters[1]);
						sleep(1);
					}
					while(((inOuts[2]+1)%BUF_SIZE) == inOuts[3]);//buffer is full paren cant write to child
					for(int i = 0; i < PARAMETERS_SIZE ; i++)
						childBuffer[inOuts[2]][i] = parameters[i] ; //writing to child
					inOuts[2] = (inOuts[2]+1)%BUF_SIZE;
				}
			}
		}
	}
}
///////////////**********************END QUESTION 2 *********************/////////////////////////////////////////////
///////////////**********************START QUESTION 3 *********************///////////////////////////////////////////

void doF321Question3(int nums[])
{
	pthread_t threads[NTHREADS];
	int* return_values = (int*)malloc(NTHREADS*sizeof(int));
	if(!return_values)
	{
		printf("malloc error\n");
		return;

	}
	for (int i = 0; i < NTHREADS; i++)
	{

		if(pthread_create(&threads[i],NULL, (void*)doF321_Thread,(int*)&nums[i]) != 0)
		{
			fprintf(stderr, "error: Cannot create thread # %d\n", i);
			exit(EXIT_FAILURE);
		}
	}
	for (int i = 0; i < NTHREADS; i++)
	{
		if (pthread_join(threads[i], (int*)&return_values[i]) != 0)
		{
			fprintf(stderr, "error: Cannot join thread # %d\n", i);
			exit(EXIT_FAILURE);
		}
	}

	 printf("Now all threads are done-the results:\n");
	 for (int i = 0; i < NTHREADS; ++i)
	 {
		 printf("number :%d, total iterations: %d\n",nums[i],return_values[i]);
	 }
	 free(return_values);
}
///////////////**********************END QUESTION 3 *********************///////////////////////////////////////////
///////////////**********************START QUESTION 4 *********************///////////////////////////////////////////


void doF321Question4(int nums[]){
	int* returns_values = (int*)mmap(NULL, total_numbers*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS,0,0);
	if(returns_values==MAP_FAILED)
	{
		printf("Mapping returns_values failed!\n");
		exit(EXIT_FAILURE);
	}
	for(int i = 0; i < total_numbers; i++)
	{
		pid_t proc_num = fork();
		if(proc_num == -1)
		{
			perror("fork failed");
			exit(EXIT_FAILURE);
		}if(proc_num == 0)
		{
			int n1 = nums[i];
			int n = nums[i];
			int k = 0;
			 while (1)
			 {
				 if (n<= 1)
				 {
					 printf("Process for n1 = %d, Number of Iterations: %d\n",n1 ,k);
					 returns_values[i] = k;
					 exit(EXIT_SUCCESS);
				 }
			        k += 1;
			        if(n % 2 == 0)
			        {
			        	n  = n / 2 ;//# Step A
			        }else
			        {
			        	n = 3 * n + 1;// # step B
			        }
			 }
		}
	}
	for(int i = 0; i< total_numbers; i++)
	{
		wait(NULL);
	}
	for(int i = 0; i< total_numbers; i++)
	{
		printf("Number %d, total iterations: %d\n",nums[i],returns_values[i]);
	}
	int err = munmap(returns_values,total_numbers*sizeof(int));
	if(err != 0)
	{
		printf("UnMapping returns_values failed\n");
		exit(EXIT_FAILURE);
	}
	return;
}
///////////////**********************END QUESTION 4 *********************///////////////////////////////////////////
///////////////**********************START QUESTION 5 *********************///////////////////////////////////////////
void doF321Question5(int nums[])
{
	//values[0] = index, values[1] = number,values[2] = return values*
	int** values = (int**)malloc(3*sizeof(int*));
	if(!values)
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	values[0] = (int*)malloc(sizeof(int));
	if(!values[0])
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	*values[0] = 0;
	values[1] = (int*)malloc(sizeof(int));
	if(!values[1])
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	values[2] = (int*)malloc(total_numbers*sizeof(int));
	if(!values[2])
	{
		printf("malloc failed\n");
		exit(EXIT_FAILURE);
	}
	sem_init(&mutex, 0, 1);
	pthread_t threads[NTHREADS];
	for (int i = 0; i < NTHREADS; i++)
	{
		*values[1] = nums[i];

		if(pthread_create(&threads[i],NULL, (void*)doF321_Semaphore,(int**)values) != 0)
		{
			fprintf(stderr, "error: Cannot create thread # %d\n", i);
			exit(EXIT_FAILURE);
		}
		sleep(1);
	}
	for (int i = 0; i < NTHREADS; i++)
	{
		pthread_join(threads[i], NULL);
	}
	for(int i = 0; i< total_numbers; i++)
	{
		printf("Number %d, total iterations: %d\n",nums[i],values[2][i]);
	}
	free(values[0]);
	free(values[1]);
	free(values[2]);
	free(values);
	sem_destroy(&mutex);
}
///////////////**********************END QUESTION 5 *********************///////////////////////////////////////////




