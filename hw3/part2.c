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
int cmpfuncrev (const void* a, const void* b)
{
   return *(int*)b - *(int*)a;
}

//function to swap 2 integers
void swap(int* a, int* b)
{
    if (*a != *b)
    {
        // *a = (*a ^ *b);
        // *b = (*a ^ *b);
        // *a = (*a ^ *b);
        // (Marginally) faster and works if a==b :
        int tmp = *a;
        *a = *b;
        *b = tmp;
        return;
    }
}

// Interval length between two integers function
int dist(int a, int b) { return abs(a - b); }

// Shell sort (better insertion sort for longer arrays)
// Supports subarrays
void shellsort(int a[], int l, int r, __compar_fn_t cmp)//, bool descending)
{
    int n = r - l;
    if (n < 2) { return; }
    for (int h = n < 701 ? 701 : n; h > 0; h/=2.3)
    {
        for (int i = h+l; i < r; i++)
        {
            int current = a[i];
            // for (int j = i; j >= h+l && (descending ? a[j-h]<current : a[j-h]>current); j-=h)
            for (int j = i; j >= h+l && cmp((void*)&current, (void*)&a[j-h]) < 0; j-=h)
            {
                swap(&a[j], &a[j-h]);
            }
        }
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

//access the disk location in SSTF
void accessSSTF(int* request, int numRequest)
{
    //write your logic here
    /**
     * Pick direction of closest request to START
     * Find closest request among remaining ones
     * Do this for the chosen request, for every request consecutively
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

    // Find min and max request
    int min = INT_MAX;
    int max = INT_MIN;
    for (int i = 0; i < numRequest; i++)
    {
        min = request[i] < min ? request[i] : min;
        max = request[i] > max ? request[i] : max;
    }

    /**
     * Sort them. All SCAN and LOOK variants are pretty much two sorted sequences
     * around the START point, so sorting will be common.
     */
    qsort(request, numRequest, sizeof(int), cmpfunc);

    int newCnt = numRequest + 1;
    int* newRequest = realloc(request, sizeof(int)*newCnt);

    // Find position of START relative to sorted requests
    int idxStart = 0;
    while (newRequest[idxStart] < START && idxStart < numRequest) { idxStart++; }

    // Move towards nearest end
    bool ascending = HIGH-START <= START-LOW;
    if (ascending)
    {
        for (int i = idxStart; i < numRequest; i++)
        {
            swap(&newRequest[i], &newRequest[i-idxStart]);
        }
        int idxNew = numRequest - idxStart;
        shellsort(newRequest, idxNew, numRequest, cmpfuncrev);
        for (int i = idxNew+1; i < newCnt; i++)
        {
            newRequest[i] = newRequest[i-1];
        }
        newRequest[idxNew] = HIGH;
    }
    else
    {
        shellsort(newRequest, 0, idxStart, cmpfuncrev);
        for (int i = idxStart+1; i < newCnt; i++)
        {
            newRequest[i] = newRequest[i-1];
        }
        newRequest[idxStart] = LOW;
    }
    puts("\n----------------");
    printf("SCAN :");
    printSeqNPerformance(newRequest, newCnt);
    // free(newRequest);
    puts("----------------");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int* request, int numRequest)
{
    //write your logic here
    int newCnt = numRequest + 2; // or +0
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
    int newCnt = numRequest;
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
    int newCnt = numRequest + 1; // or +0
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

    // int request[] = {51,52,54,55,56,57,58};
    // int request[] = {48,49,50,51,52,54,55};
    /**
     * TEST
     * SSTF : 53->54->55->52->59->10->9->8->7->6->1
     * SCAN : 53->52->10->9->8->7->6->1->LOW->54->55->59
     */
    // int request[] = {59,54,55,52,1,10,6,7,9,8};
    /**
     * TEST
     * SSTF : 53 -> 65 -> 67 -> 37 -> 14 -> 98 -> 122 -> 124 -> 183
     * SCAN : 53 -> 37 -> 14 -> LOW -> 65 -> 67 -> 98 -> 122 -> 124 -> 183
     * C-SCAN : 53 -> 65 -> 67 -> 98 -> 122 -> 124 -> 183 -> HIGH -> LOW -> 14 -> 37
     * LOOK : 53 -> 65 -> 67 -> 98 -> 122 -> 124 -> 183 -> 37 -> 14
     * LOOK : 53 -> 37 -> 14 -> 65 -> 67 -> 98 -> 122 -> 124 -> 183
     * C-LOOK : 53 -> 65 -> 67 -> 98 -> 122 -> 124 -> 183 -> LOW -> 14 -> 37
     * C-LOOK : 53 -> 37 -> 14 -> HIGH -> 183 -> 124 -> 122 -> 98 -> 67 -> 65
     */
    int request[] = { 98, 183, 37, 122, 14, 124, 65, 67 };
    
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
