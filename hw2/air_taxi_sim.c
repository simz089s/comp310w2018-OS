/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: Simon Zheng
//McGill ID: 260744353

 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
// #define INT_MIN (1 << (sizeof(int)*CHAR_BIT-1))
// #define INT_MAX (-(INT_MIN+1))

int BUFFER_SIZE = 100; //size of queue

// Semaphores
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t full;
// sem_t empty; Airplane passengers would take the bus

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
    return ((queue->size) >= queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue* queue){
    if (queue->size == 0){
        return;
    }

    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf(" Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }

}

struct Queue* queue;

/* Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers
 * to the taxi platform */
void* FnAirplane(void* cl_id)
{
    int plane_id = *(int*)cl_id;
    // struct timespec ts;
    while (true)
    {
        int nump = rand() % 6 + 5; // 5 to 10 passengers
        printf("Airplane %d arrives with %d passengers\n", plane_id, nump);
        for (int pnum = 0; pnum < nump; pnum++)
        {
            int passenger = 1000000 + (plane_id * 1000) + pnum; // Passenger ID
            printf("Passenger %d of airplane %d arrives to platform\n", passenger, plane_id);
            // sem_wait(&empty); Airplane passengers would take the bus
            pthread_mutex_lock(&mutex);
            if (isFull(queue))
            {
                printf("Platform is full: Rest of passengers of plane %d take the bus\n", plane_id);
                pthread_mutex_unlock(&mutex);
                break; // Since *rest* of passengers take the bus
            }
            enqueue(queue, passenger);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
        }
        sleep(1);
        // ts.tv_sec = 1;
        // ts.tv_nsec = 0;
        // nanosleep(&ts, NULL);
    }
    return 0;
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger 
 * home and come back to the airport */
void* FnTaxi(void* pr_id)
{
    int taxi_id = *(int*)pr_id;
    struct timespec ts;
    ts.tv_sec = 0;
    while (true)
    {
        printf("Taxi driver %d arrives\n", taxi_id);
        if (sem_trywait(&full) == -1)
        {
            printf("Taxi drive %d waits for passengers to enter the platform\n", taxi_id);
        }
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int client = dequeue(queue);
        printf("Taxi driver %d picked up client %d from the platform\n", taxi_id, client);
        pthread_mutex_unlock(&mutex);
        // sem_post(&empty); Airplane passengers would take the bus
        // sleep() only supports integers
        ts.tv_nsec = rand() % 333333334 + 166666667; // 0.16... to 0.5 secs
        nanosleep(&ts, NULL); // Simulating 10 to 30 minutes
    }
    return 0;
}

int main(int argc, char* argv[])
{

    int num_airplanes;
    int num_taxis;

    num_airplanes=atoi(argv[1]);
    num_taxis=atoi(argv[2]);

    printf("You entered: %d airplanes per hour\n",num_airplanes);
    printf("You entered: %d taxis\n", num_taxis);


    //initialize queue
    queue = createQueue(BUFFER_SIZE);

    //declare arrays of threads and initialize semaphore(s)
    pthread_t airplanes[num_airplanes];
    pthread_t taxis[num_taxis];
    // pthread_mutex_init(&mutex, NULL); Already using PTHREAD_MUTEX_INITIALIZER
    sem_init(&full, 0, 0);
    // sem_init(&empty, 0, BUFFER_SIZE); Airplane passengers would take the bus

    //create arrays of integer pointers to ids for taxi / airplane threads
    int* taxi_ids[num_taxis];
    int* airplane_ids[num_airplanes];
    // Since we are using int* arrays and passing the pointers, not the values
    int nums[num_taxis > num_airplanes ? num_taxis : num_airplanes];
    for (int i = 0; i < sizeof(nums); i++) { nums[i] = i; }

    // Seeding with time(NULL) is too imprecise for calling rand() every milliseconds
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec * t.tv_sec);

    //create threads for airplanes
    for (int i = 0; i < num_airplanes; i++)
    {
        pthread_t airplane;
        airplane_ids[i] = &nums[i];
        printf("Creating airplane thread %d\n", i);
        while (pthread_create(&airplane, NULL, FnAirplane, (void*)airplane_ids[i]));
        airplanes[i] = airplane;
    }

    //create threads for taxis
    for (int i = 0; i < num_taxis; i++)
    {
        pthread_t taxi;
        taxi_ids[i] = &nums[i];
        while (pthread_create(&taxi, NULL, FnTaxi, (void*)taxi_ids[i]));
        taxis[i] = taxi;
    }

    // Wait for threads to finish and join before destroying semaphores
    for (int i = 0; i < num_airplanes; i++) { pthread_join(airplanes[i], NULL); }
    for (int i = 0; i < num_taxis; i++) { pthread_join(taxis[i], NULL); }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&full);
    // sem_destroy(&empty); Airplane passengers would take the bus

    pthread_exit(NULL);
}
