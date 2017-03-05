#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Zhicheng Pan, 104639463

// Defining a single linked data struct
struct intNode
{
	//used to store the integer
	int value;
	//used to point to the next integer node
	struct intNode *ptrNext;	
};

//defining a function to allocate memory for a new integer node
struct intNode *createIntNode();

//defining a function to check if the integers list is empty 
int isEmpty(struct intNode *ptrFirst);

//defining a function to print out the integers list
void printIntList(struct intNode * ptrFirst);

//defining a function to free memory that used to store the integers list
void clearIntList(struct intNode * ptrFirst);


int main()
{

	//tmpInput is used to store integer that end users input
	int tmpInput,loopFlag=1;

	//tmpPtr is used to point to the current integer node
	//ptrFirst is used to point the first integer node
	//ptrLast is used to point the last integer node
	struct intNode *tmpPtr,*ptrFirst=NULL,*ptrLast=NULL;

	//used to store the 'ENTER KEY'
	char enKey;

	do{
		printf("Please enter an integer number(Any 'non-number' key to exit):  ");

		//end user's input will complete if they enter a non-number key
		if(scanf("%d%c", &tmpInput, &enKey) != 2 || enKey != '\n')
		{
			loopFlag=0;
			printf("Input completed! ");

		}
		//integers from end users input will be stored in a single linked integers list
		else
		{
			printf("You enter a valid integer! \n");
				//allocate memory and create integer node 
				tmpPtr=createIntNode();

				//handling the first node that will be created
				if (isEmpty(ptrFirst))
				{

					tmpPtr->value=tmpInput;
					tmpPtr->ptrNext=NULL;
					ptrFirst=tmpPtr;
					ptrLast=tmpPtr;

				}
				//handling the other nodes that will be created
				else
				{

					tmpPtr->value=tmpInput;
					tmpPtr->ptrNext=NULL;

					//update previous node's ptrNext to point to current node 
					ptrLast->ptrNext=tmpPtr;
					
					//update the ptrLast to be current node 
					ptrLast=tmpPtr;

				}
		
		}


	
	}while(loopFlag);

	if (ptrFirst==NULL)
	{
		printf("You didn't enter any integer, integers list is not created! \n");
		return 1;
	
	}
	else
	{	
		printf("The integers list is as below: \n");
		printf("----------------------------------------------------------------\n");
		printIntList(ptrFirst);
		printf("\n----------------------------------------------------------------\n");
		clearIntList(ptrFirst);
		printf("----------------------------------------------------------------\n");
		printf("All memory has been released, program exit!\n");
		return 0;

	};




}

struct intNode *createIntNode()
{
	return (struct intNode*) (malloc (sizeof(struct intNode)));
}

int isEmpty(struct intNode *ptrFirst)
{
	if(ptrFirst == NULL)
		return 1;
	else 
		return 0;
}

void printIntList(struct intNode * ptrFirst)
{
	// base case: empty list
	if (ptrFirst == NULL)
		return;
	else
	{
		printf("%d ", ptrFirst->value);
		printIntList(ptrFirst->ptrNext);
	}
}

// user must clear ptrFirst and ptrLast
void clearIntList(struct intNode * ptrFirst)
{
	// base case: empty list
	if (ptrFirst == NULL)
		return;
	else
	{
		clearIntList(ptrFirst->ptrNext);
		printf("It is going to free memory for: %d \n", ptrFirst->value);
		free(ptrFirst);
	}
}
