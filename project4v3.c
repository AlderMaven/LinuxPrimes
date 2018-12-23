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
#include <sys/wait.h>
#include <errno.h>

#include<semaphore.h>

/*Semids for POSIX semaphores*/
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

/*Use of global variable allows me to keep track of number of children running with the name catcher*/
/*I recognize there is a risk involved with this, but I wanted to go the simplest route to get the catcher to work*/
int currentChildCount = 0;

static void name_catcher(int signo, siginfo_t *act,  void *oldact){
	if(signo == SIGCHLD){ /*Catch the child signal*/
		wait(&signo); /*Get the exit status*/
		
		if(signo){ /*If status was non-zero then an error occured and program should and will terminate*/
			fprintf(stderr, "Due to child error program will now exit\n");
			exit(EXIT_FAILURE);
		}
		/*Otherwise drop child count so as to allow more*/
		currentChildCount--;
		
	}
	/*Don't want the no useage warnings, but have no real reason to use them*/
	if(act == oldact){
		printf("Well that's rather odd \n");
	}
}

int primeChecker(int testNumber, int list[], int counter);
int addPrime(int primes[], int newPrime, int *position);
void initializePipes(int fd[], int pipes[], int pipeCounter);
void createParentFile(int primes[], char parentFileName[], int counter, int fd, char newline[], char tempString[], int temp, int numberOfPrimes);
void readFromChildPipe(int whichChild, int pipes[], int * individualPrimeHolder, int temp, int sizeOfInt);
int midFind(int primes[], int lowBound, int highBound, int middle, int pivot, int i, int j);
void quickSort(int primes[], int lowBound, int highBound, int middle, int pivot, int i, int j);


int main(){
	/*Don't need the actual id so this is a glorified boolean*/
	pid_t isParent;
	
	/*Number of children created in total, living and dead*/
	int totalChildCount = 0; 
	
	/*Had to make my code even nastier, huh?*/
	int firstChildLowBound = 1001;
	int firstChildUpBound = 100001;
	
	/*Lower and Upperbounds for later children, is adjusted by parent to match before next fork*/
	int lowerBound = 100001;
	int upperBound = 200001;
	
	/*Lots of spare ints to use in functions or loops as needed*/
	int pipeCounter = 0;
	int functionCounter = 0;
	int counter = 0;
	int checker;
	int temp = 0;
	
	int isPrime;
	
	int lengthNumber; /*To get proper printing off of sprintf of digits*/
	int forkFlag = 0; /*Further explained in parent section of while loop*/
	int partialBytesWritten = 0; /*This was for a safety net, couldn't bring myself to remove it*/

	int fd = 0;
	
	
	
	
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
	int pipes[20];/*I decided to use multiple pipes to try to avoid overflows, 
	but it does result in higher memory/resource consumption
	and additionally because of the number of pipes, I decided not to close them in the children*/
	int fdHolder[2];
	int individualPrimeHolder = 0;
	
	char startOfChildFileName[] = "child";
	char endOfChildFileName[] = ".txt";
	char fullChildName[12];
	
	char parentFileName[] = "stdout.txt";
	char newline[] = "\n";
	char tempString[6];

	/*See close below*/
	int sizeOfInt;
	
	struct sigaction name;
		
		
	/*Set the necessary info for sigaction*/
	sigemptyset(&name.sa_mask);
	name.sa_flags = SA_SIGINFO;
	name.sa_sigaction = name_catcher;
		
	/*Catching termination of children to adjust currentChildCount, wish I had figured this out for the shell, alas thus is life*/
	sigaction(SIGCHLD, &name, NULL);
		
	sizeOfInt = sizeof(individualPrimeHolder); /*Will be using this value extremely frequently so am storing it for speed*/
	
	
	

	
	
	
	
	memset(primesList, 0, sizeof(primesList));
	
	
	
	
	/*Add the first 5 primes as mentioned in project to the primes list*/
	addPrime(primesList, 2, numberOfPrimes);
	addPrime(primesList, 3, numberOfPrimes);
	addPrime(primesList, 5, numberOfPrimes);
	addPrime(primesList, 7, numberOfPrimes);
	addPrime(primesList, 11, numberOfPrimes);
	
	/*Initialize all those semaphores, goes on for 50 lines if you want to skim, yes refactoring deadline also yes*/
	temp = sem_init(&sem0, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem1, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem2, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem3, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem4, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem5, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem6, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem7, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem8, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
	}
	temp = sem_init(&sem9, 0, 1);
	if(temp == -1){
		fprintf(stderr, "Error in semaphore creation \n exiting with status failure");
		exit(EXIT_FAILURE);
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
	
	
	
	
		/*I don't need more than 10 children but I also don't want to exit this loop before I finish reading off the last child*/
		/*This may be risky if somehow the last child to fork off manages to write all and have all it's numbers read before the other 2 remaining ones can*/
		/*I found that to incredibly unlikely, but I figured I should mention it*/
	while(totalChildCount < 10 || childTenPipeFilled == 1){
		
		
		if(currentChildCount < 3 && totalChildCount < 10){ /*Don't make more if at 3, or if at 10 total*/
			currentChildCount++;
			isParent = fork();
			forkFlag = 1;
		}
		
		
		if(isParent){ /*Add the ==1 if you really need to*/
			
			if(totalChildCount > 0 && forkFlag){ /*Fork flag to make sure this only triggers after a child is made*/
				/*Thankfully the pattern allows this to work*/
				lowerBound += 100000;
				upperBound += 100000;
				/*reset the flag*/
				forkFlag = 0;
				/*iterate the number of children up*/
				totalChildCount += 1;
			}
			
			
			if(totalChildCount == 0){
				totalChildCount++;
			}
			
			
			
			switch(totalChildCount){ /*Since I iterate totalChildCount before I reach here this switch works properly, these flags used for pipe reading to show that there is useful data in pipe*/
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
				default: fprintf(stderr, "You shouldn't be in the default \n case is %d \n", totalChildCount);
				break;
			}
			
			
			
			
			
			
			/*Doing non-exclusive ifs so that I can try to keep each pipe with data from filling, this will result in a scrambled list of primes however
			So I have to quicksort later which costs performance, but I must fit the specifications*/
			/*This goes on until for about 300 lines with similar logic in each if so I'll explain the first and then I'd recommend skimming the rest*/
			/*Very good spot for refactoring revision*/
			if(childOnePipeFilled == 1){
				
				
				temp = sem_wait(&sem0); /*Using semaphores to avoid partial reads/writes*/
				
				if(temp == -1){ /*Just to catch semaphore failures*/
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(0, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child
				since the readpipe = 2*child starting from child 0*/
				
				
				temp = sem_post(&sem0);
				
				if(temp == -1){ /*Just to catch semaphore failures*/
					fprintf(stderr, "error in parent sempost\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				if(individualPrimeHolder != -1 && individualPrimeHolder != 0){ /*The != 0 can likely be removed here, but I have it as a failsafe currently, -1 explained below*/
				addPrime(primesList, individualPrimeHolder, numberOfPrimes);
				}
				else if(individualPrimeHolder == -1 ){ /*I used a write of -1 into the pipe as a signal that the child was done writing
				since -1 should never appear in the pipe, hence I stop trying to read from the pipe once -1 read*/
					childOnePipeFilled = 0; 
					close(pipes[0]);
				}
			}
			if(childTwoPipeFilled  == 1){
				temp = sem_wait(&sem1);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(2, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childThreePipeFilled  == 1){
				temp = sem_wait(&sem2);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(4, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childFourPipeFilled == 1){
				temp = sem_wait(&sem3);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(6, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childFivePipeFilled  == 1){
				temp = sem_wait(&sem4);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(8, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childSixPipeFilled == 1){
				temp = sem_wait(&sem5);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(10, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childSevenPipeFilled == 1){
				temp = sem_wait(&sem6);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(12, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
				
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
			if(childEightPipeFilled == 1){
				temp = sem_wait(&sem7);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(14, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childNinePipeFilled == 1){
				temp = sem_wait(&sem8);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(16, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			if(childTenPipeFilled == 1){
				
				temp = sem_wait(&sem9);
				
				if(temp == -1){
					fprintf(stderr, "error in parent semwait\n exiting with failure");
					exit(EXIT_FAILURE);
				}
				
				
				readFromChildPipe(18, pipes, &individualPrimeHolder, temp, sizeOfInt); /*Constant here represents which child*/
				
				
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
			/*
			END
			OF
			THE
			READS
			YES
			IT
			NEEDS
			REFACTORING
			I
			AGREE
			*/
				
		
			
		
			
			
			
		}
		else{
			/*For 1st child */ 
			if(totalChildCount == 0){ /*First child doesn't fit the pattern of the rest so I gave it an exceptional case here*/
				/*Print to stderr on create*/
				fprintf(stderr, "Starting child %d \n", (totalChildCount));
				
				
				/*Hard coded since I know the name of the first child*/
				strcpy(fullChildName, startOfChildFileName);
				strcat(fullChildName, "0");
				strcat(fullChildName, endOfChildFileName);
				
				
				/*Open the file for writing*/
				checker = open(fullChildName, O_CREAT|O_APPEND|O_RDWR|O_TRUNC, S_IRWXU|S_IROTH|S_IRGRP);
				if(checker != -1){
					for(counter = firstChildLowBound; counter < firstChildUpBound;){ /*For every other number within the bound check if prime*/
						isPrime = primeChecker(counter, primesList, functionCounter);
						if(isPrime){/*if num is prime, make it into a string and print it to file*/
							lengthNumber = sprintf(tempString, "%d", counter);
							write(checker, tempString, lengthNumber);
							write(checker, newline, 1);
							/*Lock pipe until written to avoid partials*/
							sem_wait(&sem0);
							
							/*Write back to parent pipe*/
							temp = write(pipes[((totalChildCount*2)+1)], &counter, sizeOfInt);
							
							/*Honestly with the semaphore this could probably be removed since it should never be executed, but it's a relatively harmless safety net to keep*/
							while(temp < sizeOfInt){
								partialBytesWritten = write(pipes[((totalChildCount*2)+1)], &counter, sizeOfInt-temp);
								temp = temp+partialBytesWritten;
							}
							/*Release semaphore*/
							sem_post(&sem0);
							
							
							/*Pipe prime back, add to child file*/
						}
						counter+=2;
					}
					/*Write the string*/
					fprintf(stderr, "Ending child %d \n", (totalChildCount));
					/*Place the flag in the pipe*/
					counter = -1;
					write(pipes[1], &counter, sizeof(counter));
					/*Exit out to be caught by sigaction*/
					exit(EXIT_SUCCESS);
				}
				else{
					/*Catch incase program is not allowed to overwrite or create files in directory*/
					printf("Failed to open file for child %d\n Will not calculate primes for its set \n", totalChildCount);
					exit(EXIT_FAILURE);
				}
			}
			else{
				
				fprintf(stderr, "Starting child %d \n", (totalChildCount));
				
				/*Dynamically make the name using sprintf*/
				strcpy(fullChildName, startOfChildFileName);
				temp = totalChildCount;
				sprintf(tempString, "%d", temp); /*Used tempString since I had it for the primes anyway and figured I wouldn't make a second array just for this*/
				strcat(fullChildName, tempString);
				strcat(fullChildName, endOfChildFileName);
				
				
				/*Same logic for open as above*/
				checker = open(fullChildName, O_CREAT|O_APPEND|O_RDWR|O_TRUNC, S_IRWXU|S_IROTH|S_IRGRP);
				if(checker != -1){
					for(counter = lowerBound; counter < upperBound;){ /*Same logic on bounds*/
						isPrime = primeChecker(counter, primesList, functionCounter);
						if(isPrime){
							lengthNumber = sprintf(tempString, "%d", counter); /*Same logic for file writes*/
							write(checker, tempString, lengthNumber);
							write(checker, newline, 1);
							/*Dynamically select which sem and pipe to use based on child, really just for the sem though since the pipe could instead be determined
							based on the totalChildCount for the process*/
							switch(totalChildCount){ /*Used switch for speed since these are mutually exclusive*/
								case 1:
								sem_wait(&sem1);
								write(pipes[3], &counter, sizeof(counter));
								sem_post(&sem1);
								break;
								case 2:
								sem_wait(&sem2);
								write(pipes[5], &counter, sizeof(counter));
								sem_post(&sem2);
								break;
								case 3:
								sem_wait(&sem3);
								write(pipes[7], &counter, sizeof(counter));
								sem_post(&sem3);
								break;
								case 4:
								sem_wait(&sem4);
								write(pipes[9], &counter, sizeof(counter));
								sem_post(&sem4);
								break;
								case 5:
								sem_wait(&sem5);
								write(pipes[11], &counter, sizeof(counter));
								sem_post(&sem5);
								break;
								case 6:
								sem_wait(&sem6);
								write(pipes[13], &counter, sizeof(counter));
								sem_post(&sem6);
								break;
								case 7:
								sem_wait(&sem7);
								write(pipes[15], &counter, sizeof(counter));
								sem_post(&sem7);
								break;
								case 8:
								sem_wait(&sem8);
								write(pipes[17], &counter, sizeof(counter));
								sem_post(&sem8);
								break;
								case 9:
								sem_wait(&sem9);
								write(pipes[19], &counter, sizeof(counter));
								sem_post(&sem9);
								break;
								default:
								fprintf(stderr, "Hit default state for semaphore switch");
								break;
							}
							
						}
						counter+=2; /*Still add 2 to skip past those evens*/
					}
					/*Sending the message back*/
					fprintf(stderr, "Ending child %d \n", (totalChildCount));
					/*and the flag for the pipe*/
					counter = -1;
					write(pipes[((totalChildCount*2)+1)], &counter, sizeof(counter));
					exit(EXIT_SUCCESS);
				}
				else{
					printf("Failed to open file for child %d as it already exists\n Will not calculate primes for its set \n", totalChildCount);
					exit(EXIT_FAILURE);
				}
			}
		}
		
	}
	
	
	
	
	
	
	
	
	
	
	checker = *numberOfPrimes; /*Ensure checker is initialized*/
	
	
	/*Create full prime output by sorting the list*/
	/*Reusing the ints here rather than making distinct ones for the function, 
	you may wish to replace with more indicative variable names instead*/
	quickSort(primesList, temp, checker, counter, totalChildCount, firstChildLowBound, firstChildUpBound);
	
	/*create parent output file and fill with now sorted list*/
	createParentFile(primesList, parentFileName, counter, fd, newline, tempString, temp, *numberOfPrimes);
	/*Remove this if it hurts my time since it is not strictly necessary by the projects parameters
	Not a massive improvement, especially with the sort hogging but it helps*/
	
	counter = 0;
	checker = *numberOfPrimes;
	
	/*I like the == 0 sometimes too*/
	while(primesList[counter] == 0){ /*List has a number of zeroes in front after quickSort, so move past them*/
		counter++;
	}
	while(counter < checker+1){ /*Number of primes is one short of actual count*/
		printf("%d\n", primesList[counter]);
		counter++;
	}
    
	/*Time to release all the remaining resources*/
	free(numberOfPrimes);
	
	
	exit(EXIT_SUCCESS);
}
/*End of main*/






















/*Detemines if number is prime via modulus of all primes less than or equal to it's square root*/
int primeChecker(int testNumber, int list[], int counter){
	int root = (int)sqrt(testNumber);
	
	
	int returnFlag = 1; 
	for(counter = 1; list[counter] <= root; ++counter){
		if((testNumber % list[counter]) == 0){
			returnFlag = 0;
			return returnFlag;
		}
	}
	return returnFlag;
}

/*add found prime to list*/
int addPrime(int primes[], int newPrime, int* position){
	
   primes[*position] = newPrime;
  
   *position = *position + 1;
   if(!position){
    printf("This should never execute");
   }
   return 1;
}

/*think this one is self-evident, but it makes all the pipes and places them in the array*/
void initializePipes(int fd[], int pipes[], int pipeCounter){
	pipe(fd);
	pipes[(pipeCounter*2)] = fd[0];
	pipes[((pipeCounter*2)+1)] = fd[1];
}

/*I wanted to output parent with all primes to a file as well for testing purposes and felt like I could leave it in for fun */
void createParentFile(int primes[], char parentFileName[], int counter, int fd, char newline[], char tempString[], int temp, int numberOfPrimes){
	counter = 1;

	fd = open(parentFileName, O_CREAT | O_APPEND | O_RDWR | O_TRUNC, S_IRWXU);
	if(fd != -1){
		while(counter < numberOfPrimes+1){
			temp = sprintf(tempString, "%d", primes[counter]);
			write(fd, tempString, temp);
			write(fd, newline, sizeof(char));
			counter++;
		}
		
	}
	else{
		fprintf(stderr, "Failed to open parent file"); /*I don't exit fail since frankly this has no bearing on the required outputs*/
	}
}




/*How I read from a given pipe, sem is locked before we get here*/
void readFromChildPipe(int whichRead, int pipes[], int * individualPrimeHolder, int temp, int sizeOfInt){
	temp = read(pipes[whichRead], individualPrimeHolder , sizeOfInt);
	if(temp == -1){
		if(errno != 4){ /*System interrupt sometimes causes the read to not execute properly, causing a doubling of the number in the list*/
		fprintf(stderr, "Error in reading from child %d \n The value given is %d \n errno is %d \n error code is %s \n", whichRead/2, *individualPrimeHolder, errno, strerror(errno));
		}
		else{
			temp = read(pipes[whichRead], individualPrimeHolder , sizeOfInt); /*Hence I read again here to move the cursor forward for real while keeping the number*/
		}
	}
	/*This shouldn't execute with the semaphores, emphasis on shouldn't*/
	else if(temp < sizeOfInt){
		while(temp < sizeOfInt){
			fprintf(stderr, "partial read \n");
		}
	}
	
	return;
}
/*sorting primes list, it's quick sort you know how it goes*/
void quickSort(int primes[], int lowBound, int highBound, int middle, int pivot, int i, int j){

	if(lowBound < highBound){ /*Stop recursive nightmare*/
		middle = midFind(primes, lowBound, highBound, middle, pivot, i, j); /*Find a proper middle*/
		
		quickSort(primes, middle+1, highBound, middle, pivot, i, j);/*Divide and conquer */
		quickSort(primes, lowBound, middle-1, middle, pivot, i, j);/*Eventually veni, vidi, vinci*/
	}


}
/*Find the middle for quickSort*/
int midFind(int primes[], int lowBound, int highBound, int middle, int pivot, int i, int j){
	pivot = primes[highBound]; /*Start with pivot at the end*/
	i = lowBound - 1; /*Start as low as possible*/
	
    for (j = lowBound; j <= highBound-1; j++){ /*Iterate through the set*/
        if (primes[j] <= pivot){ /*Swap times ahead*/
            i++; /*No -1 access here*/
			middle = primes[i];
			primes[i] = primes[j];
			primes[j] = middle;
        }
    }
	/*One last swap*/
	middle = primes[i+1];
	primes[i+1] = primes[highBound];
	primes[highBound] = middle;
    return (i+1); /*Return position*/
}




