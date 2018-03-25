/**
 * Student name : Simon Zheng
 * Student ID   : 260744353
 */

// TODO: REMOVE TEST CODE!!!

#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc
#include <limits.h>

#ifndef __INT_MAX__
#define __INT_MAX__ (-((1 << (sizeof(int)*CHAR_BIT-1))+1))
#endif

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function 
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc (const void* a, const void* b)
{
   return *(int*)a - *(int*)b;
}

//function to swap 2 integers
void swap(int* a, int* b)
{
    if (*a != *b)
    {
        // *a = (*a ^ *b);
        // *b = (*a ^ *b);
        // *a = (*a ^ *b);
        int tmp = *a;
        *a = *b;
        *b = tmp;
        return;
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int* request, int numRequest)
{
    int acc = 0;
    int last = START;
    puts("");
    printf("%d", START);
    for (int i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int* request, int numRequest)
{
    //simplest part of assignment
    puts("\n----------------");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    puts("----------------");
    return;
}

// Interval length between two integers function
int dist(int a, int b) { return abs(a - b); }

//access the disk location in SSTF
void accessSSTF(int* request, int numRequest)
{
    //write your logic here
    /**
     * Pick direction of closest request to START
     * If greater, sort all numbers greater than START to righthand side
     * and descending order for requests smaller than START to lefthand side
     * Vice-versa if starting request is smaller
     *
     * Here I just used a naive bruteforce algorithm that finds the closest
     * element right of the current one going from left to right (smaller to
     * bigger) index in the array
     */
    int nearestDist = abs(request[0] - START);
    for (int i = 1; i < numRequest; i++)
    {
        int distance = abs(request[i] - START);
        if (distance < nearestDist)
        {
            nearestDist = distance;
            swap(&request[0], &request[i]);
        }
    }
    for (int i = 0; i < numRequest; i++)
    {
        nearestDist = INT_MAX;
        for (int j = i+1; j < numRequest; j++)
        {
            int distance = abs(request[i] - request[j]);
            if (distance < nearestDist)
            {
                nearestDist = distance;
                swap(&request[j], &request[i+1]);
            }
        }
    }
    puts("\n----------------");
    printf("SSTF :");
    printSeqNPerformance(request, numRequest);
    puts("----------------");
    return;
}

//access the disk location in SCAN
void accessSCAN(int* request, int numRequest)
{
    
	//write your logic here
    puts("\n----------------");
    printf("SCAN :");
    // printSeqNPerformance(newRequest, newCnt);
    puts("----------------");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int* request, int numRequest)
{
    //write your logic here
    puts("\n----------------");
    printf("CSCAN :");
    // printSeqNPerformance(newRequest, newCnt);
    puts("----------------");
    return;
}

//access the disk location in LOOK
void accessLOOK(int* request, int numRequest)
{
    //write your logic here
    puts("\n----------------");
    printf("LOOK :");
    // printSeqNPerformance(newRequest, newCnt);
    puts("----------------");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int* request, int numRequest)
{
    //write your logic here
    puts("\n----------------");
    printf("CLOOK :");
    // printSeqNPerformance(newRequest,newCnt);
    puts("----------------");
    return;
}

int main()
{
    // int* request;
    int numRequest, ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    // scanf("%d", &numRequest);
    // request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    // for (int i = 0; i < numRequest; i++)
    {
        // scanf("%d", &request[i]);
    }

    // TEST (should be 53->54->55->52->59->10->9->8->7->6->1)
    int request[] = {59,54,55,52,1,10,6,7,9,8};
    numRequest = sizeof(request)/sizeof(typeof(request[0]));

    puts("\nSelect the policy : ");
    puts("----------------");
    puts("1\t FCFS");
    puts("2\t SSTF");
    puts("3\t SCAN");
    puts("4\t CSCAN");
    puts("5\t LOOK");
    puts("6\t CLOOK");
    puts("----------------");
    scanf("%d", &ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1: accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2: accessSSTF(request, numRequest);
        break;

    //access the disk location in SCAN
    case 3: accessSCAN(request, numRequest);
        break;

    //access the disk location in CSCAN
    case 4: accessCSCAN(request, numRequest);
        break;

    //access the disk location in LOOK
    case 5: accessLOOK(request, numRequest);
        break;

    //access the disk location in CLOOK
    case 6: accessCLOOK(request, numRequest);
        break;

    default:
        break;
    }
    return EXIT_SUCCESS;
}
