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
#include <sys/wait.h> /*Needed for temp wait implementation*/

int primeChecker(int testNumber, int list[], int counter);
int addPrime(int primes[], int newPrime, int *position);
void initializePipes(int fd[], int pipes[], int pipeCounter);
void createParentFile(int primes[], char parentFileName[], int counter, int fd, char newline[]);
void catPrimeFiles(char parentFileName[], char fullChildName[], char startOfChildFileName[], char endOfChildFileName[], int counter, char catCommand[], char numberHolder);
int main(){
	
	pid_t isParent;
	
	int totalChildCount = 0;
	int currentChildCount = 0;
	int firstChildLowBound = 1001;
	int firstChildUpBound = 100001;
	int lowerBound = 200001;
	int upperBound = 300001;
	int counter = 0;
	int pipeCounter = 0;
	int functionCounter = 0;
	int checker;
	int fd = 0;
	int isPrime;
	int temp;
	int lengthNumber;

	
	/*These could just be in a int array, but I wanted to be more clear on this*/
	/*int childOnePipeFilled = 0;
	int childTwoPipeFilled = 0;
	int childThreePipeFilled = 0;
	int childFourPipeFilled = 0;
	int childFivePipeFilled = 0;
	int childSixPipeFilled = 0;
	int childSevenPipeFilled = 0;
	int childEightPipeFilled = 0;
	int childNinePipeFilled = 0;
	int childTenPipeFilled = 0;*/
	int * numberOfPrimes = (int *) malloc(4);
	int initialPrimes[100000];
	int pipes[20];
	
	int fdHolder[2];
	char startOfChildFileName[] = "child";
	char endOfChildFileName[] = ".txt";
	char fullChildName[12];
	char catCommand[250]; /*This is arbitrarily long at the moment*/
	char parentFileName[] = "stdout.out";
	char newline[] = "\n";
	char numberHolder;
	char tempString[6];
	
	
	
	
	memset(initialPrimes, 0, sizeof(initialPrimes));
	
	
	/*Add the first 5 primes as mentioned in project to the primes list*/
	addPrime(initialPrimes, 2, numberOfPrimes);
	addPrime(initialPrimes, 3, numberOfPrimes);
	addPrime(initialPrimes, 5, numberOfPrimes);
	addPrime(initialPrimes, 7, numberOfPrimes);
	addPrime(initialPrimes, 11, numberOfPrimes);
	
	
	while(pipeCounter < 10){
		
		initializePipes(fdHolder, pipes, pipeCounter);
		pipeCounter++;
	}
	
	
	/*Iterates through from 13 through 1000, adding all primes to the list*/
	for(counter = 13; counter < 1001;){
	
	  isPrime = primeChecker(counter, initialPrimes, functionCounter);
	  
	  if(isPrime){
		  
		  addPrime(initialPrimes, counter, numberOfPrimes);
	  }
	  counter += 2;
	}
	
	createParentFile(initialPrimes, parentFileName, counter, fd, newline);
		
	while(totalChildCount < 10){
		
		
		if(currentChildCount != 3){
			isParent = fork();
		}
		
		
		if(isParent != 0 && isParent != -1){
			
			if(totalChildCount > 0){
				lowerBound += 100000;
				upperBound += 100000;
			}
			printf("%d", totalChildCount);
			
			currentChildCount++;
			totalChildCount += 1;
			
			isParent = -1;
			
			wait(NULL); /*For testing prior to pipe read implementation*/
			currentChildCount--;
		}
		else if(isParent != -1){
			/*For 1st child */ 
			if(totalChildCount == 0){
				/*Print to stderr on create*/
				fprintf(stderr, "Starting child %d \n", (totalChildCount+1));
				
				strcpy(fullChildName, startOfChildFileName);
				strcat(fullChildName, "0");
				strcat(fullChildName, endOfChildFileName);
				
				
				
				checker = open(fullChildName, O_CREAT | O_APPEND | O_RDWR | O_TRUNC, S_IRUSR | S_IRUSR | S_IROTH | S_IRGRP);
				if(checker != -1){
					for(counter = firstChildLowBound; counter < firstChildUpBound;){
						isPrime = primeChecker(counter, initialPrimes, functionCounter);
						if(isPrime){
							lengthNumber = sprintf(tempString, "%d", counter);
							write(checker, tempString, lengthNumber);
							write(checker, newline, 1);
							write(pipes[((totalChildCount*2)+1)], &counter, sizeof(counter));
							/*Pipe prime back, add to child file*/
						}
						counter+=2;
					}
					fprintf(stderr, "Ending child %d \n", (totalChildCount+1));
					exit(EXIT_SUCCESS);
				}
				else{
					printf("Failed to open file for child %d \n Will not calculate primes for its set \n", totalChildCount);
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
						isPrime = primeChecker(counter, initialPrimes, functionCounter);
						if(isPrime){
							lengthNumber = sprintf(tempString, "%d", counter);
							write(checker, tempString, lengthNumber);
							write(checker, newline, 1);
							write(pipes[((totalChildCount*2)+1)], &counter, sizeof(counter));
							/*Pipe prime back*/
						}
						counter+=2;
					}
					fprintf(stderr, "Ending child %d \n", (totalChildCount+1));
					exit(EXIT_SUCCESS);
				}
				else{
					printf("Failed to open file for child %d \n Will not calculate primes for its set \n", totalChildCount);
					exit(EXIT_SUCCESS);
				}
			}
		}
		
	}
	//Create full prime output
	catPrimeFiles(parentFileName, fullChildName, startOfChildFileName, endOfChildFileName, counter, catCommand, numberHolder);

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
void 