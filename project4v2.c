/*CS 3377.002 Project 3 bmb160230 Brandon Barnes*/
/*Prime Calculator*/

/*Notes*/
/*There will be no more than 100,000 primes*/
/*Any prime < 1,000,000 can be determined by comparing to the primes found in the first 1,000 numbers*/
/*If a number is not cleanly divisible by the numbers less than it's square root it is not prime*/
/*There are 168 primes in the first 1,000 numbers*/
/*No even number > 2 is prime*/
/*This means that we can start each child at it's (respective multiple of 100,000) +1 then add 2*/
/*This should greatly increase efficiency*/


/*Primary functionality*/
/*Has a parent calculate the primes from the integer number set up to 1000*/
/*Parent will then spawn 3 children each of which will calculate the primes for a set of 100,000 numbers*/
/*An exception for the first child of course which will find them in 1,001 to 100,000 inclusive*/
/*Whenever a child process finishes a new one will be made for the next 100,000 set not currently being used*/
/*This will continue until all primes in the set of [2-1,000,000] have been found*/
/*Parent will then print these numbers to stdout in ascending order with 1 prime per line*/

/*Other functionality:*/
/*Each child will write its primes to a permanent file as well as to a pipe back to the parent*/
/**/

/*Will be assuming that primes in prompt can be added to list without issue*/

#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

#include<semaphore.h>

sem_t sem0;
sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;
sem_t sem6;
sem_t sem7;
sem_t sem8;
sem_t sem9;


const int semkey = 451; /*Key for system 5 semaphore, pastiching Fahrenheit 451 as per tradition*/

int primeChecker(int testNumber, int list[], int counter);
int addPrime(int primes[], int newPrime, int *position);
void initializePipes(int fd[], int pipes[], int pipeCounter);
void createParentFile(int primes[], char parentFileName[], int counter, int fd, char newline[]);
void semaphore_p(int semid, int whichSem, struct sembuf sops[], int counter);
void semaphore_v(int semid, int whichSem, struct sembuf sops[], int counter);
void readFromChildPipe(int whichChild, int pipes[], int * individualPrimeHolder, int temp, int sizeOfInt);

int main(){
	
	pid_t isParent;
	
	int totalChildCount = 0;
	int currentChildCount = 0;
	int firstChildLowBound = 1001;
	int firstChildUpBound = 100001;
	int lowerBound = 100001;
	int upperBound = 200001;
	int counter = 0;
	int pipeCounter = 0;
	int functionCounter = 0;
	int checker;
	int fd = 0;
	int isPrime;
	int temp = 0;
	int lengthNumber;
	int forkFlag = 0;
	int semid;
	int waitCatcher;
	int waitStatus;

	
	
	
	
	
	/*These could just be in a int array, but I wanted to be more clear on this*/
	int childOnePipeFilled = 0;
	int childTwoPipeFilled = 0;
	int childThreePipeFilled = 0;
	int childFourPipeFilled = 0;
	int childFivePipeFilled = 0;
	int childSixPipeFilled = 0;
	int childSevenPipeFilled = 0;
	int childEightPipeFilled = 0;
	int childNinePipeFilled = 0;
	int childTenPipeFilled = 0;
	
	
	
	
	
	
	
	int * numberOfPrimes = (int *) malloc(4);
	int primesList[100000];
	int pipes[20];/*I decided to use multiple pipes to try to avoid overflows, this may be reduceable, which would allow for some other performance changes later on*/
	int fdHolder[2];
	int individualPrimeHolder = 0;
	
	char startOfChildFileName[] = "child";
	char endOfChildFileName[] = ".txt";
	char fullChildName[12];
	char catCommand[250]; /*This is arbitrarily long at the moment*/
	char parentFileName[] = "stdout.out";
	char newline[] = "\n";
	char tempString[6];
	
	union semun semunInst;
	struct sembuf sops[1];
	
	int sizeOfInt;
	sizeOfInt = sizeof(individualPrimeHolder); /*Will be using this value extremely frequently so am storing it for speed*/
	
	
	

	
	
	
	
	memset(primesList, 0, sizeof(primesList));
	
	/*semunInst.val = 1;
	semid = semget(semkey, 10, 0600 | IPC_CREAT);
	for(counter = 0; counter <10; counter++){ /*Set all semaphores to One
		semctl(semid, counter, SETVAL, semunInst);
	}
	
	
	
	
	/*Add the first 5 primes as mentioned in project to the primes list*/
	addPrime(primesList, 2, numberOfPrimes);
	addPrime(primesList, 3, numberOfPrimes);
	addPrime(primesList, 5, numberOfPrimes);
	addPrime(primesList, 7, numberOfPrimes);
	addPrime(primesList, 11, numberOfPrimes);
	
	temp = sem_init(&sem0, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem1, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem2, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem3, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem4, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem5, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem6, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem7, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem8, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	temp = sem_init(&sem9, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
	}
	
	while(pipeCounter < 10){
		
		initializePipes(fdHolder, pipes, pipeCounter);
		pipeCounter++;
	}
	
	
	/*Iterates through from 13 through 1000, adding all primes to the list*/
	for(counter = 13; counter < 1001;){
	
	  isPrime = primeChecker(counter, primesList, functionCounter);
	  
	  if(isPrime){
		  
		  addPrime(primesList, counter, numberOfPrimes);
	  }
	  counter += 2;
	}
	
	
	
	createParentFile(primesList, parentFileName, counter, fd, newline);
		
	while(totalChildCount < 10 || childTenPipeFilled){
		
		
		if(currentChildCount != 3 && !childTenPipeFilled){
			isParent = fork();
			forkFlag = 1;
		}
		
		
		if(isParent){
			
			if(totalChildCount > 0 && forkFlag){
				lowerBound += 100000;
				upperBound += 100000;
				forkFlag = 0;
			}
			
			
			
			currentChildCount++;
			totalChildCount += 1;
			
			switch(totalChildCount){ /**/
				case 1 :
				childOnePipeFilled = 1;
				break;
				case 2 :
				childTwoPipeFilled = 1;
				break;
				case 3 :
				childThreePipeFilled = 1;
				break;
				case 4 :
				childFourPipeFilled = 1;
				break;
				case 5 :
				childFivePipeFilled = 1;
				break;
				case 6 :
				childSixPipeFilled = 1;
				break;
				case 7 :
				childSevenPipeFilled = 1;
				break;
				case 8 :
				childEightPipeFilled = 1;
				break;
				case 9 :
				childNinePipeFilled = 1;
				break;
				case 10 :
				childTenPipeFilled = 1;
				break;
				default: fprintf(stderr, "You shouldn't be in the default \n");
			}
			/*Doing non-exclusive ifs so that I can try to keep each pipe with data from filling, this will result in a scrambled list of primes however*/
			
			if(childOnePipeFilled){
				
				
				temp = sem_wait(&sem0);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(0, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem0);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childOnePipeFilled = 0;
				}
			}
			if(childTwoPipeFilled){
				temp = sem_wait(&sem1);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(1, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem1);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childTwoPipeFilled = 0;
				}
				
			}
			if(childThreePipeFilled){
				temp = sem_wait(&sem2);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(2, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem2);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childThreePipeFilled = 0;
				}
			}
			if(childFourPipeFilled){
				temp = sem_wait(&sem3);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(3, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem3);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childFourPipeFilled = 0;
				}
				
			}
			if(childFivePipeFilled){
				temp = sem_wait(&sem4);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(4, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem4);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childFivePipeFilled = 0;
				}
				
			}
			if(childSixPipeFilled){
				temp = sem_wait(&sem5);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(5, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem5);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childSixPipeFilled = 0;
				}
			}
			if(childSevenPipeFilled){
				temp = sem_wait(&sem6);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(6, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem6);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childSevenPipeFilled = 0;
				}
				
			}
			if(childEightPipeFilled){
				temp = sem_wait(&sem7);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(7, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem7);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childEightPipeFilled = 0;
				}
				
			}
			if(childNinePipeFilled){
				temp = sem_wait(&sem8);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(8, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem8);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childNinePipeFilled = 0;
				}
				
			}
			if(childTenPipeFilled){
				
				temp = sem_wait(&sem9);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(9, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				temp = sem_post(&sem9);
				
				if(temp == -1){
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){
					childTenPipeFilled = 0;
				}
				
			}
			/*will catch any wait return without hanging, if there is not one returning then waitCatcher will be zero,
			this will let me keep 3 children open without having to clear their pipe first*/
			/*If this doesn't work then a sigchild handler will be in order*/
			waitCatcher = waitpid(-1, &waitStatus ,WNOHANG);
			if(waitCatcher){
				currentChildCount--;
			}
		
			
			
			
		}
		else{
			/*For 1st child */ 
			if(totalChildCount == 0){
				/*Print to stderr on create*/
				fprintf(stderr, "Starting child %d \n", (totalChildCount));
				
				strcpy(fullChildName, startOfChildFileName);
				strcat(fullChildName, "0");
				strcat(fullChildName, endOfChildFileName);
				
				
				
				checker = open(fullChildName, O_CREAT | O_APPEND | O_RDWR | O_TRUNC, S_IRUSR | S_IROTH | S_IRGRP);
				if(checker != -1){
					for(counter = firstChildLowBound; counter < firstChildUpBound;){
						isPrime = primeChecker(counter, primesList, functionCounter);
						if(isPrime){
							lengthNumber = sprintf(tempString, "%d", counter);
							write(checker, tempString, lengthNumber);
							write(checker, newline, 1);
							sem_wait(&sem0);
							fprintf(stderr, "past semwait in child 0"); /*Still need error checking*/
							write(pipes[((totalChildCount*2)+1)], &counter, sizeof(counter));
							sem_post(&sem0);
							fprintf(stderr, "past sempost in child 0");
							
							/*Pipe prime back, add to child file*/
						}
						counter+=2;
					}
					fprintf(stderr, "Ending child %d \n", (totalChildCount));
					counter = -1;
					write(pipes[1], &counter, sizeof(counter));
					exit(EXIT_SUCCESS);
				}
				else{
					printf("Failed to open file for child %d as it already exists\n Will not calculate primes for its set \n", totalChildCount);
					exit(EXIT_SUCCESS);
				}
			}
			else{
				
				fprintf(stderr, "Starting child %d \n", (totalChildCount+1));
				
				
				strcpy(fullChildName, startOfChildFileName);
				temp = totalChildCount;
				sprintf(tempString, "%d", temp);
				strcat(fullChildName, tempString);
				strcat(fullChildName, endOfChildFileName);
				
				
				
				checker = open(fullChildName, O_CREAT | O_APPEND | O_RDWR | O_TRUNC, S_IRUSR | S_IRUSR | S_IROTH | S_IRGRP);
				if(checker != -1){
					for(counter = lowerBound; counter < upperBound;){
						isPrime = primeChecker(counter, primesList, functionCounter);
						if(isPrime){
							lengthNumber = sprintf(tempString, "%d", counter);
							write(checker, tempString, lengthNumber);
							write(checker, newline, 1);
							switch(totalChildCount){
								case 1:
								sem_wait(&sem1);
								break;
								case 2:
								sem_wait(&sem2);
								break;
								case 3:
								sem_wait(&sem3);
								break;
								case 4:
								sem_wait(&sem4);
								break;
								case 5:
								sem_wait(&sem5);
								break;
								case 6:
								sem_wait(&sem6);
								break;
								case 7:
								sem_wait(&sem7);
								break;
								case 8:
								sem_wait(&sem8);
								break;
								case 9:
								sem_wait(&sem9);
								break;
								default:
								fprintf(stderr, "Hit default state for semaphore switch");
								break;
							}
							
							/*semaphore_p(semid, totalChildCount, sops, counter); /*Constant here represents which semaphore*/
							
							write(pipes[((totalChildCount*2)+1)], &counter, sizeof(counter));
							
							switch(totalChildCount){
								case 1:
								sem_post(&sem1);
								break;
								case 2:
								sem_post(&sem2);
								break;
								case 3:
								sem_post(&sem3);
								break;
								case 4:
								sem_post(&sem4);
								break;
								case 5:
								sem_post(&sem5);
								break;
								case 6:
								sem_post(&sem6);
								break;
								case 7:
								sem_post(&sem7);
								break;
								case 8:
								sem_post(&sem8);
								break;
								case 9:
								sem_post(&sem9);
								break;
								default:
								fprintf(stderr, "Hit default state for semaphore switch");
								break;
							}
							
							/*semaphore_v(semid, totalChildCount, sops, counter); /*Constant here represents which semaphore*/
							
						}
						counter+=2;
					}
					fprintf(stderr, "Ending child %d \n", (totalChildCount+1));
					counter = -1;
					write(pipes[((totalChildCount*2)+1)], &counter, sizeof(counter));
					exit(EXIT_SUCCESS);
				}
				else{
					printf("Failed to open file for child %d as it already exists\n Will not calculate primes for its set \n", totalChildCount);
					exit(EXIT_SUCCESS);
				}
			}
		}
		
	}
	/*Create full prime output*/

	return 0;
}

int primeChecker(int testNumber, int list[], int counter){
	int root = sqrt(testNumber);
	int returnFlag = 1; 
	for(counter = 1; list[counter] <= root; ++counter){
		if((testNumber % list[counter]) == 0){
			returnFlag = 0;
			return returnFlag;
		}
	}
	return returnFlag;
}

int addPrime(int primes[], int newPrime, int* position){
	
   primes[*position] = newPrime;
  
   *position = *position + 1;
   if(!position){
    printf("This should never execute");
   }
   return 1;
}

void initializePipes(int fd[], int pipes[], int pipeCounter){
	pipe(fd);
	pipes[(pipeCounter*2)] = fd[0];
	pipes[((pipeCounter*2)+1)] = fd[1];
}

void createParentFile(int primes[], char parentFileName[], int counter, int fd, char newline[]){
	counter = 0;

	fd = open(parentFileName, O_CREAT | O_APPEND | O_RDWR | O_TRUNC, S_IRWXU);
	if(fd != -1){
		while(primes[counter] != 0){
			write(fd, &primes[counter], sizeof(counter));
			write(fd, newline, sizeof(char));
			counter++;
		}
		
	}
	
}

/*
void semaphore_p(int semid, int whichSem, struct sembuf sops[], int counter){
	/*Wait/decrement/lock
	sops[0].sem_num = whichSem;       
    sops[0].sem_op = 0;        
    sops[0].sem_flg = 0;
	fprintf(stderr, "p called \n");
	counter = semop(semid, sops, 1);
	
	if(counter){
		fprintf(stderr, "An error has occured with the sem lock\n now exiting");
		exit(EXIT_FAILURE);
	}
    sops[0].sem_num = whichSem;       
    sops[0].sem_op = -1;        
    sops[0].sem_flg = 0;
	
	counter = semop(semid, sops, 1);
	
	
	return;
}


void semaphore_v(int semid, int whichSem, struct sembuf sops[], int counter){
	/*Post/increment/unlock
    sops[0].sem_num = whichSem;
    sops[0].sem_op = 1;        
    sops[0].sem_flg = 0;
	fprintf(stderr, "v called \n");
	counter = semop(semid, sops, 1);
	
	if(counter){
		fprintf(stderr, "An error has occured with the sem unlock\n now exiting");
		exit(EXIT_FAILURE);
	}
	return;
}
*/



void readFromChildPipe(int whichChild, int pipes[], int * individualPrimeHolder, int temp, int sizeOfInt){
	temp = read(pipes[whichChild*2], individualPrimeHolder , sizeOfInt);
	if(temp == -1){
		fprintf(stderr, "Error in reading from child %d \n exiting with failure", whichChild);
		exit(EXIT_FAILURE);
	}
	
	return;
}
/*sorting and output to prime file*/