#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

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
    for (int j = 0; j < numRes; j++)
        { request_vector[j] = rand() % (Need[pr_id][j] + 1); }
}

/*\
 | Implementation of isSafe() as described in the slides
\*/
bool isSafe()
{
    bool isSafe = false;
    int Work[numRes];
    int Finish[numRes];

    // Step 1:
    for (int j = 0; j < numRes; j++)
    {
        Work[j] = Avail[j];
    }
    for (int i = 0; i < numProc; i++)
    {
        Finish[i] = false;
    }

    int found = -1;
    // bool allFinished = false;
    while (!isSafe)
    {
        // Step 2:
        for (int i = 0; found == -1 && i < numProc; i++)
        {
            // Loop through P[i]s or as long as not found (-1)
            if (!Finish[i])
            {
                /* Check if every Need[i][j] <= Work[j] for P[i]
                 * Else go to next P[i+1] (next outer loop iteration since found == -1) */
                for (int j = 0; found == -1 && j < numRes; j++)
                    { if (Need[i][j] > Work[j]) { break; } }
                found = i; // If every Need[i][j] <= Work[j] for P[i] go to step 3
            }
        }
        if (found == -1) { break; } // If no adequate P[i] found go to step 4

        // Step 3:
        // "Allocate" resources to found P
        for (int j = 0; j < numRes; j++) { Work[j] += Hold[found][j]; }
        Finish[found] = true; // Flag found P as finished
        // Go to step 2 again
        found = -1; // Reset found
    }
    // Step 4:
    // Turns out I didn't need bool isSafe but I'll leave it
    for (int i = 0; i < numProc; i++)
        { if (!Finish[i]) { return false; } }
    return true;
}

/*\
\|/ Implementation of Bankers Algorithm as described in the slides
/|\ returns 1 if safe allocation 0 if not safe
\*/
bool bankers_algorithm(int pr_id, int* request_vector)
{
    while (true) {
        // Step 1:
        for (int j = 0; j < numRes; j++)
            { if (request_vector[j] > Need[pr_id][j]) { return -1; } }

        // Step 2:
        pthread_mutex_lock(&mutex);
        // CS
        bool reqAvail = true; // Assume initially requested is available
        for (int j = 0; reqAvail && j < numRes; j++)
            { if (request_vector[j] > Avail[j]) { reqAvail = false; } }
        // If requested not available, go to step 1 (beginning of loop)
        if (!reqAvail) {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        // Step 3:
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
            // Else go back to step 1
            for (int j = 0; j < numRes; j++)
            {
                // Cancel provisional allocations
                Avail[j] += request_vector[j];
                Hold[pr_id][j] -= request_vector[j];
                Need[pr_id][j] += request_vector[j];
            }
            pthread_mutex_unlock(&mutex);
            return false;
        }
    }

    return true;
}

/*\
 | Simulates processes running on the system.
\*/
void* process_simulator(void* pr_id)
{
    int th_id = *(int*)pr_id;
    int request_vector[numRes];
    while (true)
    {
        request_simulator(th_id, request_vector);
        while (!bankers_algorithm(th_id, request_vector));
        for (int j = 0; j < numRes; j++)
        {
            // Check if cannot terminate
            if (Need[th_id][j] != 0 || Hold[th_id][j] != Max[th_id][j]) { break; }
            else
            {
                // If can terminate, release/free resources
                pthread_mutex_lock(&mutex);
                for (int k = 0; k < numRes; k++) { Avail[k] += Hold[th_id][k]; }
                pthread_mutex_unlock(&mutex);
                // Terminate
                return NULL;
            }
        }
        sleep(3); // Sleep before making other request if cannot terminate
    }
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
    Avail = malloc(numRes * sizeof(int));
    for (int i = 0; i < numRes; i++)
        { scanf("%d", &Avail[i]); }

    puts("Enter Maximum Resources Each Process Can Claim:");
    Max = malloc(numProc * sizeof(int));
    for (int i = 0; i < numProc; i++)
    {
        Max[i] = malloc(numRes * sizeof(int));
        for (int j = 0; j < numRes; j++)
        {
            scanf("%d", &Max[i][j]);
        }
    }

    Hold = malloc(numProc * sizeof(int));
    for (int i = 0; i < numProc; i++) {
        Hold[i] = malloc(numRes * sizeof(int));
        for (int j = 0; j < numRes; j++) {
            Hold[i][j] = 0; } }

    Need = malloc(numProc * sizeof(int));
    for (int i = 0; i < numProc; i++) {
        Need[i] = malloc(numRes * sizeof(int));
        for (int j = 0; j < numRes; j++) {
            Need[i][j] = Max[i][j]; } }

    printf("Output:\nThe Number of each resource in the system is: ");
    for (int i = 0; i < numRes; i++)
        { printf("%d ", Avail[i]); }

    printf("\nThe Allocated Resources table is:\n");
    for (int i = 0; i < numProc; i++) {
        for (int j = 0; j < numRes; j++) { printf("%d ", Hold[i][j]); }
        puts("");
    }

    printf("The Maximum Claim table is:\n");
    for (int i = 0; i < numProc; i++) {
        for (int j = 0; j < numRes; j++) { printf("%d ", Max[i][j]); }
        puts("");
    }

    printf("The Available Resources array is: ");
    for (int i = 0; i < numRes; i++)
        { printf("%d ", Avail[i]); }
    puts("");

    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec * t.tv_sec);

    sem_init(&full_count, 0, 0);
    sem_init(&empty_count, 0, numProc);

    pthread_t procs[numProc];
    int ids[numProc];

    //create threads simulating processes (process_simulator)
    for (int i = 0; i < numProc; i++)
    {
        ids[i] = i;
        while (pthread_create(&procs[i], NULL, process_simulator, (void*)&ids[i]));
    }

    //create a thread that takes away resources from the available pool (fault_simulator)

    //create a thread to check for deadlock (deadlock_checker)

    for (int i = 0; i < numProc; i++) { pthread_join(procs[i], NULL); }
    pthread_mutex_destroy(&mutex);
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
