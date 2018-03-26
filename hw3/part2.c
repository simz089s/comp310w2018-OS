/**
 * Student name : Simon Zheng
 * Student ID   : 260744353
 */

// TODO: REMOVE TEST CODE!!!

#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc
#include <limits.h>

#include <stdbool.h>

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

// Shell sort (better insertion sort for longer arrays)
// Supports subarrays and descending order flag
void shellsort(int a[], int l, int r, bool descending)
{
    if (l >= r-1 || l < 0 || r < 0) { return; }
    int n = r - l;
    for (int h = n < 701 ? 701 : n; h > 0; h/=2.3)
    {
        for (int i = h+l; i < r; i++)
        {
            int current = a[i];
            for (int j = i; j >= h+l && (descending ? a[j-h]<current : a[j-h]>current); j-=h)
            {
                swap(&a[j], &a[j-h]);
            }
        }
    }
}

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
     * bigger) index in the array for every element consecutively
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
    int min = INT_MAX;
    int max = INT_MIN;
    for (int i = 0; i < numRequest; i++)
    {
        min = request[i] < min ? request[i] : min;
        max = request[i] > max ? request[i] : max;
    }
    shellsort(request, 0, numRequest, false);
    int newCnt = numRequest;
    int l = 0;
    int r = numRequest;
    bool addLOW = (request[0] != LOW);
    bool addHIGH = (request[numRequest-1] != HIGH);
    int* newRequest = malloc(sizeof(int)*(newCnt + addLOW + addHIGH));
    int idx = 0;
    while (request[idx] < START) { idx++; }
    // bool ascending = HIGH-idx <= idx-LOW ? true : false;
    bool ascending = max-START <= START-min ? true : false;
    if (ascending)
    {
        for (int i = idx; i < numRequest; i++)
            { swap(&request[i], &request[i-idx]); }
        int idx2 = numRequest - idx;
        shellsort(request, idx2, numRequest, true);
        int i = 0;
        for (; i < idx2; i++)
        {
            newRequest[i] = request[i];
        }
        if (addHIGH)
        {
            newRequest[i] = HIGH;
            l++;
            r++;
            newCnt++;
        }
        for (i = addHIGH?idx2+1:idx2; i < r; i++)
        {
            if (addHIGH) { newRequest[i] = request[i-l]; }
            else { newRequest[i] = request[i-l]; }
        }
        if (addLOW)
        {
            request[i] = LOW;
            newCnt++;
        }
    }
    else
    {
        if (addLOW)
        {
        }
        shellsort(request, 0, idx, true);
        int i = 0;
        for (; i < idx; i++)
        {
            newRequest[i] = request[i];
        }
        if (addLOW)
        {
            newRequest[i] = LOW;
            l++;
            r++;
            newCnt++;
        }
        for (i = addLOW?idx+1:idx; i < r; i++)
        {
            newRequest[i] = request[i-l];
        }
        if (addHIGH)
        {
            newRequest[i] = HIGH;
            newCnt++;
        }
    }
    puts("\n----------------");
    printf("SCAN :");
    printSeqNPerformance(newRequest, newCnt);
    free(newRequest);
    puts("----------------");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int* request, int numRequest)
{
    //write your logic here
    // shellsort(request, 0, numRequest, false);
    // shellsort(request, 0, numRequest, true);
    // int* newRequest = request;
    // int newCnt = numRequest;
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
    // int idx = 0;
    // while (request[idx] < START) { idx++; }
    // bool goRight = request[numRequest]-idx < idx-request[0] ? true : false;
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

    /**
     * TEST
     * SSTF : 53->54->55->52->59->10->9->8->7->6->1
     * SCAN : 53->54->55->59->HIGH->52->10->9->8->7->6->1->LOW
     */
    int request[] = {59,54,55,52,1,10,6,7,9,8};
    // int request[] = {51,52,54,55,56,57,58}; // descending
    // int request[] = {48,49,50,51,52,54,55}; // ascending
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
