#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

int main(/*int argc, char* argv[argc]*/)
{
    int numProc, numRes;

    printf("Enter number of processes: ");
    if (scanf("%d", &numProc) < 1) { return EXIT_FAILURE; }

    printf("\nEnter number of resources: ");
    if (scanf("%d", &numRes) < 1) { return EXIT_FAILURE; }

    int availRes[4096/sizeof(int)] = {0,};
    while ()

    int resources[distRes][2];
    for (int i = 0; i < distRes; i++)
    {
        int  = strtoimax(argv[2], (char**)NULL, 10);
    }
    return EXIT_SUCCESS;
}
