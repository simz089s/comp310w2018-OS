#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>

// Vectors and matrices
char buf[4096];
int numProc, numRes;
int* Avail;
int** Max;
int** Hold;
int** Need;
// int** Req;

// Semaphores
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t full_count;
sem_t empty_count;

/*\
 | Simulates resource requests by processes 
\*/
void request_simulator(int pr_id, int* request_vector)
{

}

/*\
 | Implementation of isSafe() as described in the slides
\*/
bool isSafe()
{

    bool isSafe = false;
    return isSafe;
}

/*\
\|/ Implementation of Bankers Algorithm as described in the slides
/|\ returns 1 if safe allocation 0 if not safe
\*/
int bankers_algorithm(int pr_id, int* request_vector)
{
    while (true) {
        // Step 1:
        for (int j = 0; j < numRes; j++)
            { if (request_vector[j] > Need[pr_id][j]) { return -1; } }
        
        // Step 2:
        bool reqAvail = true; // Assume initially requested is available
        for (int j = 0; reqAvail && j < numRes; j++)
            { if (request_vector[j] > Avail[j]) { reqAvail = false; } }
        // If requested not available, go to step 1 (beginning of loop)
        if (!reqAvail) { continue; }

        // Step 3:
        pthread_mutex_lock(&mutex);
        // CS
        for (int j = 0; j < numRes; j++)
        {
            // Provisional allocations
            Avail[j] -= request_vector[j];
            Hold[pr_id][j] += request_vector[j];
            Need[pr_id][j] -= request_vector[j];
        }
        if (isSafe())
        {
            // Resources granted; done
            pthread_mutex_unlock(&mutex);
            break;
        }
        else
        {
            // Else go back to step 1 (beginning of loop)
            for (int j = 0; j < numRes; j++)
            {
                // Cancel provisional allocations
                Avail[j] += request_vector[j];
                Hold[pr_id][j] -= request_vector[j];
                Need[pr_id][j] += request_vector[j];
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    return 0;
}

/*\
 | Simulates processes running on the system.
\*/
void* process_simulator(void* pr_id)
{

}

/*\
 | Simulates a fault occuring on the system.
\*/
void* fault_simulator(void* pr_id)
{
    // TODO
    return pr_id;
}

/*\
 | Checks for deadlock
\*/
void* deadlock_checker()
{
    // TODO
    return NULL;
}

int main(/*int argc, char* argv[argc]*/)
{
    //Initialize all inputs to banker's algorithm

    // char buf[4096];
    // int numProc, numRes;

    printf("Enter number of processes: ");
    if (scanf("%s", buf) < 1)
        { return EXIT_FAILURE; }
    else
        { numProc = strtoimax(buf, (char**)NULL, 10); }

    printf("Enter number of resources: ");
    if (scanf("%s", buf) < 1)
        { return EXIT_FAILURE; }
    else
        { numRes = strtoimax(buf, (char**)NULL, 10); }

    printf("Enter Available Resources: ");
    // int availRes[numRes];
    Avail = malloc(numRes * sizeof(int));
    for (int i = 0; i < numRes; i++)
        { scanf("%d", &Avail[i]); }

    puts("Enter Maximum Resources Each Process Can Claim:");
    // int maxRes[numProc][numRes];
    Max = malloc(numProc * sizeof(int));
    for (int i = 0; i < numProc; i++)
    {
        Max[i] = malloc(numRes * sizeof(int));
        for (int j = 0; j < numRes; j++)
        {
            scanf("%d", &Max[i][j]);
        }
    }

    // int holdRes[numProc][numRes];
    Hold = malloc(numProc * sizeof(int));
    for (int i = 0; i < numProc; i++) {
        Hold[i] = malloc(numRes * sizeof(int));
        for (int j = 0; j < numRes; j++) {
            Hold[i][j] = 0; } }

    // int needRes[numProc][numRes];
    Need = malloc(numProc * sizeof(int));
    for (int i = 0; i < numProc; i++) {
        Need[i] = malloc(numRes * sizeof(int));
        for (int j = 0; j < numRes; j++) {
            Need[i][j] = Max[i][j]; } }
    
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec * t.tv_sec);

    // int reqRes[numProc][numRes];
    // Req = malloc(numProc * sizeof(int));
    // for (int i = 0; i < numProc; i++) {
    //     Req[i] = malloc(numRes * sizeof(int));
    //     for (int j = 0; j < numRes; j++) {
    //         Req[i][j] = rand() % Need[i][j]; } }

    sem_init(&full_count, 0, 0);
    sem_init(&empty_count, 0, numProc);

    //create threads simulating processes (process_simulator)

    //create a thread that takes away resources from the available pool (fault_simulator)

    //create a thread to check for deadlock (deadlock_checker)

    for (int i = 0; i < numProc; i++)
    {
        // pthread_join();
    }
    free(Avail);
    free(Max);
    free(Hold);
    free(Need);
    return EXIT_SUCCESS;
}

/*
Enter number of processes: 3
Enter number of resources: 2
Enter Available Resources: 2 2
Enter Maximum Resources Each Process Can Claim:
2 2
2 2
2 2
Output:
The Number of each resource in the system is: 2 2
The Allocated Resources table is: 
0 0
0 0
0 0
The Maximum Claim table is: 
2 2
2 2
2 2
The Available Resources array is: 2 2
Simulating fault 
requesting resources for process 1 
The Resource vector requested array is: 0 1
Checking if allocation is safe 
System is safe: allocating
requesting resources for process 2 
The Resource vector requested array is: 2 1
Checking if allocation is safe 
Allocation is not safe, cancelling
*/
