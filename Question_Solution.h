#ifndef QUESTION_SOLUTION_H_
#define QUESTION_SOLUTION_H_

#include <pthread.h>

#define BUF_SIZE 100
#define PARAMETERS_SIZE 3


//all functions
void init_limit(int num);
int doF321_Thread(int* num);
void doF321_Semaphore(int** values);
void doF321Question1(int nums[]);
void doF321Question2(int nums[]);
void doF321Question3(int nums[]);
void doF321Question4(int nums[]);
void doF321Question5(int nums[]);

#endif /* QUESTION_SOLUTION_H_ */
