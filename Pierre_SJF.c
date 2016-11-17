#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
/*GET THE QUE TAIL TO CONNECT TO THE QUEUE HEAD!!!!!!!!!!!! Done.
//Work on the new function SJQUEUE. It brings the shortest job of the queue to the front.
*/
unsigned int sysClock; //Clock
//Mutex Stuff
pthread_mutex_t newInfo, newShortest;
pthread_cond_t condC, condW;
//Queue structure
typedef struct daQueue
{
	char name[10];
	unsigned int timeArrive;
	unsigned int length;
	char type[3];
	unsigned int timeLeft;
	unsigned int start;
	unsigned int finish;
	struct daQueue *prev;
	struct daQueue *next;
	struct daQueue *print;
}daQueue;

bool end; // Determines when the last job is created
int endTime;
void printQue(daQueue *que);
void endQueue(daQueue ***tail, daQueue *rTail, daQueue **info);

int quantum;
//Every necessary pointer and double pointer
daQueue *tHead, *mHead, *fHead;
daQueue *temporary, *tempPtr, *inCPU;//points to the job currently in CPU
daQueue *startQue, **startTail;
FILE *fpoint;

int main()
{
	startTail = &startQue;
	end = false;
	sysClock = 90000;
	quantum = 0;
	endTime = 0;
	fpoint = fopen("Processes.txt" , "r");
	
	clockWork();
	/*for(i = 0; i < 35; i++)
		newJob();	
	printQue(tHead);
	printQue(fHead);
	printQue(mHead);*/
	return 0;
}

/*void newJob()
{
	daQueue *tempHead = (daQueue *) malloc (sizeof(daQueue));
	fscanf(fpoint,"%s %d %d %d %s ", tempHead->name, &tempHead->timeArrive, &tempHead->length, &n, tempHead->type);
	tempHead->timeLeft = tempHead->length;
	if(strcmp(tempHead->type, "FP") == 0)
		sJQueue(&fHead, &fTail, *fTail, &tempHead, tempHead);
	else if(strcmp(tempHead->type,"MM")  == 0)
		sJQueue(&mHead, &mTail, *mTail, &tempHead, tempHead);
	else
		sJQueue(&tHead, &tTail, *tTail, &tempHead, tempHead);
}*/

void newJob()
{
	int n;
	daQueue *tempHead = (daQueue *) malloc (sizeof(daQueue));
	if(fscanf(fpoint,"%s %d %d %d %s ", tempHead->name, &tempHead->timeArrive, &tempHead->length, &n, tempHead->type) == 5)
	{
		tempHead->timeLeft = tempHead->length;
		temporary = *&tempHead;
	}
	else
		end = true;
}

void deleteNode(daQueue **daNodePtr, daQueue *daNode)
{
	
	if(strcmp(daNode->type, "FP") == 0)
	{
		if(fHead->next != NULL)
		{
			fHead = fHead->next;
			fHead->prev = NULL;
		}
		else
			fHead = NULL;
	}
	else if(strcmp(daNode->type,"MM")  == 0)
	{
		if(mHead->next != NULL)
		{
			mHead = mHead->next;
			mHead->prev = NULL;
		}
		else
			mHead = NULL;
	}
	else
	{
		if(tHead->next != NULL)
		{
			tHead = tHead->next;
			tHead->prev = NULL;
		}
		else
			tHead = NULL;
	}
	tempPtr = NULL;
	//endQueue(&finalTail, *finalTail, *&daNodePtr);
}
void printQue(daQueue *que)//Prints the queue that you choose in the parameter
{
	if(que != NULL)
	{
		printf("Process: %s Arrival Time: %d Length: %d Type: %s\n", que->name, que->timeArrive, que->length, que->type);
		if(que->next != NULL)
			printQue(que->next);
		else
			printf("\n");
	}
}
void endQueue(daQueue ***tail, daQueue *rTail, daQueue **info)//Adds node to the end of the print queue
{
	//**tail = (daQueue *) malloc (sizeof(daQueue));
	/*rTail->name = info->name;
	rTail->timArrive = info->timeArrive;
	rTail->length = info->length;
	rTail->type = info->type;
	rTail->start = info->start;*/
	tempPtr = *info;
	**tail = tempPtr;
	*tail = &tempPtr->print;
	tempPtr = NULL;
}

//Don't have a use for it yet


void putInQue()
{
	if(strcmp(temporary->type, "FP") == 0)//put into the correct queue
		sJQueue(&fHead);
	else if(strcmp(temporary->type,"MM")  == 0)
		sJQueue(&mHead);
	else
		sJQueue(&tHead);
	endQueue(&startTail, *startTail, &temporary);
	temporary = NULL;
}
void sJQueue(daQueue **head)//puts job in the current position of the queue
{
	if(*head == NULL)
		*head = temporary;
	else
	{
		tempPtr = *head;
		if(temporary->timeLeft < tempPtr->timeLeft)
		{
			temporary->next = tempPtr;
			tempPtr->prev = temporary;
			*head = temporary;
		}
		else if(tempPtr->next == NULL)
		{
			tempPtr->next = temporary;
			temporary->prev = tempPtr;
		}
		else
		{
			tempPtr = tempPtr->next;
			while(tempPtr->timeLeft < temporary->timeLeft && tempPtr->next != NULL)
				tempPtr = tempPtr->next;
			if(tempPtr->timeLeft < temporary->timeLeft)
			{
			
				tempPtr->next = temporary;
				temporary->prev = tempPtr;
			}
			else
			{
				tempPtr->prev->next = temporary;
				temporary->prev = tempPtr->prev;
				temporary->next = tempPtr;
				tempPtr->prev = temporary;
			}
		}
	}
	tempPtr = NULL;
}

void checkShortJob()//Checks to see out of the head of the queues which time left is the shortest
{
	if(tHead != NULL)
	{
		if(fHead != NULL && fHead->timeLeft < tHead->timeLeft)
		{
			if(mHead != NULL && mHead->timeLeft < fHead->timeLeft)
			{
				inCPU = mHead;
			}
			else
			{
				inCPU = fHead;
			}
		}
		else if(mHead != NULL && mHead->timeLeft < tHead->timeLeft)
		{
			inCPU = mHead;
		}
		else
		{
			inCPU = tHead;
		}
	}
	else if(fHead != NULL)
	{
		if(mHead != NULL && mHead->timeLeft < fHead->timeLeft)
		{
			inCPU = mHead;
		}
		else
		{
			inCPU = fHead;
		}
	}
	else if(mHead != NULL)
	{
		inCPU = mHead;
	}
	else
		inCPU = NULL;
	if(inCPU != NULL && inCPU->start == 0)
	{
		inCPU->start = sysClock;
	}
	quantum = 0;
	//printStatus(startQue);
}
void clockWork()// Where the magic happens
{
	while(true)
	{
		if(temporary == NULL)//inputs job if there is no new job
				newJob();
		if(!end)
		{
			
			if(temporary->timeArrive == sysClock)
			{
				
				putInQue();
				checkShortJob();
				printStatus(startQue);
				//sysClock++;
				/*printQue(tHead);
				printQue(fHead);
				printQue(mHead);*/
			}
			
		}
		else
		{
			endTime++; 
			if(endTime % 500 == 0)
				printStatus(startQue);
			if(inCPU == NULL)//Checks for ending of while loop
			{
				//printf("Total running time: %d\n", sysClock - 90000);
				break;
			}
			
			
		}
		sysClock++;
		quantum++;
		if(inCPU != NULL)//Checks for current job
		{
			inCPU->timeLeft--;
			if(inCPU->timeLeft == 0)
			{
				inCPU->finish = sysClock;
				deleteNode(&inCPU, inCPU);
				checkShortJob();
			}	
		}
		if(quantum == 100)
			checkShortJob();
		
		
		
	}
	printStatus(startQue);
}

void printStatus(daQueue *status)//Prints the printQueue
{
	if(status == startQue)
	printf("Name	Length	Arrival Time	Start Time	Time Left	Finish Time	Type\n");
	printf("%s	%d	%d		", status->name, status->length, status->timeArrive);
	if(status->start == 0)
		printf("NOT DONE	");
	else
		printf("%d		", status->start);
	printf("%d		", status->timeLeft);
	if(status->finish == 0)
		printf("NOT DONE	");
	else
		printf("%d		", status->finish);
	printf("%s\n", status->type);
	if(status->print != NULL)
		printStatus(status->print);
	else
		printf("\n\n");
}

/*void sJQueueRec(daQueue **headPtr, daQueue *head, daQueue **tempPtr, daQueue *temp)
{	Might work on it later in life.
	
	if(*headPtr == NULL)
		*headPtr = temp;
	temporary = head;
	if(temporary->timeLeft < temp->timeLeft)
	{
		sJQueueRec(headPtr, head->next, tempPtr, temp);
	}
	else if(!done)
	{
		if(temporary->timeLeft < temp->timeLeft)
		{
			temporary->next = *tempPtr;
			temp->prev = temporary;
		}
		else if(temporary->prev != NULL)
		{
			temporary->prev->next = *tempPtr;
			temp->prev = temporary->prev;
			temp->next = temporary;
			temporary->prev = temp;
		}
		else
		{
			temporary->prev = *tempPtr;
			temp->next = temporary;
			*headPtr = temp;
		}
		done = true;
	}
}*/
