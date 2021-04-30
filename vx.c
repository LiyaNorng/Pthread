#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>



/* buffer.h */
const int max  = 5; // this is just my constant variable
int buffer[5]; // this is my buffer, for storing items
int size = 0;
int In  = 0;  // this is for keeping track where at in the buffer is the next item to put in
int Out = 0; // this is for keeping track where at in the buffer is the next item to take out
int n = 0; // keeping track how many are in the buffer
sem_t  mutex;  // my mutex lock for keeping track who is in the buffer
sem_t  empty; // this is my variable for checking if it is empty
sem_t  full;  // this is for keeping track if the buffer is full

// this is my consumer thread, where it will call to the remove method to get the item, and display it
void * Consumer(void * i)
{
    int item = 0; // storing temporary item
    while (1)
    {
        int sleepTime = rand() % 3 + 1; // generate random number for sleep
        /* sleep for a random period of time */
        printf ("Consumer thread %ld sleeping for %d seconds.\n", (long) i, sleepTime);
        sleep(sleepTime); // telling the thread to go to sleep for sleepTime
        // this if statement is for check if it actually get to remove the item
        if (remove_Item(& item) < 0)
        {
            printf ("Consumer error\n");
        }
        else
        {
            printf ("Consumer thread %ld removed value %d.\n", (long) i , item);
        }
    }
}
// this is my producer thread, where it will generate a number, and it will call to the insert method to insert
// the item that it just generate.
void * Producer(void * i)
{
    int item = 0;  // this is my temporary storage for using to put as a parameter when calling insert method
    while(1)
    {
        /* generate random number */
        int sleepTime = rand() % 3 + 1; // generate some random number
        /* sleep for random amount of time */
        printf ("Producer thread %ld sleeping for %d seconds.\n", (long) i, sleepTime);
        sleep(sleepTime); // thread is going to sleep
        /* generate a random number - this is the producer's product */
        item = rand() % 100 + 1; // generate some number to put in the buffer, and store it in the item
        if (insert_Item(item) < 0) // this if statement is for checking if the producer actually get
        {                           // to insert the item
            printf ("Producer error\n");
        }
        else
        {
            printf ("Producer thread %ld inserted value %d.\n", (long) i , item);
        }
    }
}
// this is my insert method, it will get the item in the parameter, and it will use to store it in the buffer if it not lock
int insert_Item(int items)
{
    int i = 0;  // loop counter
    int check = 0; // for checking if the item got remove from the buffer
    int out = 0;  // running total for how many available space left are in the buffer
    int count = 0;  // running total for how many item are in the buffer
    do
    {
        /* produce an item in next produced */
        sem_wait(&empty); // lock if buffer is empty
        sem_wait(&mutex); // lock if there is a process is in the buffer
        /* add next produced to the buffer */
        if (n == 0) // this is for checking if the buffer is empty. if so, then reset the In, and Out so it won't
        {           // just store in different slot when when the buffer is empty.
            In = 0;
            Out = 0;
        }
        printf ("Insert_item inserted item %d at position %d\n",items, In);
        buffer[In] = items; // putting the item in the buffer
        In = (In + 1) % max; // this is for setting the next empty slot in the buffer
        check++;
        n++;
        for (i = 0; i < 5; i++) // this for loop is for checking the buffer for empty, and getting the item to dislpay
        {
            if (buffer[i] == (int)NULL)
            {
                printf ("[empty]");
                out++;
            }
            else if (buffer[i] != (int)NULL)
            {
                printf ("[%d]", buffer[i]);
                count++;
            }
        }
        printf ("in = %d, out = %d.\n", count, out);
        i = 0;   // reset the value for next thread
        count = 0;
        out = 0;
        // release the lock for the next thead to come in
        sem_post(&mutex);
        sem_post(&full);
        // returning the value that i get from above
        if ( check == 0)
        {
            return -1;
        }
        else
        {
            check = 0;
            return 0;
        }
    }while (1);
}
// this is my remove method, it would get the item from the buffer, and return it to the consumer, so it can display the item to the user
int remove_Item(int * items)
{
    int i = 0;  // loop counter
    int out = 0;  // running total for how many available space left are in the buffer
    int count = 0; // running total for how many item are in the buffer
    int check = 0;  // for checking if the item got remove from the buffer
    do
    {   // locking if it is full, and setting mutex
        sem_wait(&full);
        sem_wait(&mutex);
        /* remove an item from buffer to next consumed */
        printf ("Remove_item removed item %d at position %d\n",*items, Out);
        *items = buffer[Out];  // getting the item from the buffer
        buffer[Out] = (int)NULL; // setting the item that took out to NULL, reset for the next item
        Out = (Out + 1) % max; // settitng the Out to the next available space which one up
        check++; // increment check since it already remove
        n--; // this is keeping count how many are in the buffer
        // this for loop is for checking for item in the buffer, and display them
        for (i = 0; i < 5; i++)
        {
            // this if statement is checking if it is empty
            if (buffer[i] == (int)NULL)
            {
                printf ("[empty]");
                out++;
            } // so if the buffer is not empty then it will run this if statement
            else if (buffer[i] != (int)NULL)
            {
                printf ("[%d]", buffer[i]);
                count++;
            }
        } // this will print out the result getting from the for loop. Which i use out for empty, and count for
        // item in the buffer
        printf ("in = %d, out = %d.\n", count, out);
        i = 0; // resetting the counter value to 0
        count = 0; // resetting the running total of item in the buffer value to 0
        out = 0; // resetting the running total for empty to 0
        sem_post(&mutex); // release the lock for mutex
        sem_post(&empty); // release the lock for empty
        // this will return the result from removing
        if ( check == 0)
        {
            return -1;
        }
        else
        {
            check = 0;
            return 0;
        }
    }while (1);
}

int main(int argc, char *argv[])
{
    // this is for storing value return from creating pthread
    int producers, consumers;
    // this is use to store the amount of producer, and consumer
    int run = 0, numProducer = 0, numConsumer = 0;
    
    // doing if statment to check to make sure the user give the right format that it suppose to be
    if (argc != 4)
    {
        printf ("Sorry that's not the right format.\n");
        printf ("Please use this format ./name (int)seconds (int)producer (int)consumer.\n");
        exit (1);
    }
    else
    {
        // if the top if statement is false, then the else will run, and turing the value of string into int.
        run = atoi(argv[1]);  // how long the main will sleep
        numProducer = atoi(argv[2]); // how many produer to make
        numConsumer = atoi(argv[3]);  // how many consumer to make
    }
    
    /* 2. Initialize buffer */
    // creating the link to the pthread
    pthread_t producerT[numProducer];
    pthread_t consumerT[numConsumer];
    
    printf ("Main thread beginning.\n");
    
    // initialize the semaphore for full, empty, and mutex
    int em = sem_init(&empty, 0, max); // MAX buffers are empty to begin with...
    int ful = sem_init(&full, 0, 0);    // ... and 0 are full
    int mut = sem_init(&mutex, 0, 1);   // mutex = 1 because it is a lock (NEW LINE)
   
    // setting the buffer to null so easier to do if statement to check for empty
    int i = 0;  // loop counter
    for (i = 0; i < max; i++)
    {
        buffer[i] = (int)NULL;
    }
    
    /// this is for checking if the initialization of the mutex, full, and empty is fail
    if (em == -1 || ful == -1 || mut == -1)
    {
        printf("Fail to initialize the semaphore.\n");
        exit(1); // this would exit if it fail
    }
    
    /* 3. Create producer threads. */
    // creating the producer threads by doing for loop
    for (i = 0; i < numProducer; i++)
    {
        printf ("Creating producer thread with id %d.\n", i);
        fflush(stdout); // want the OS to flush the statement above
        producers = pthread_create(&producerT[i], NULL, Producer, (void*)(long)i);
        // this is for checking if the pthread is fail
        if (producers)
        {
            printf ("Error:unable to create thread, %d.\n", producers);
            exit(1); // this would exit if it fail
        }
    }
    
    /* 4. Create consumer threads.  */
    // creating the consumer thread by doing for loop
    for (i = 0; i < numConsumer; i++)
    {
        printf ("Creating consumer thread with id %d.\n", i);
        fflush(stdout);  // want the OS to flush the statement above
        consumers = pthread_create(&consumerT[i], NULL , Consumer, (void*)(long)i);
        // this is for checking if the pthread is fail
        if (consumers)
        {
            printf ("Error:unable to create thread, %d.\n", consumers);
            exit(1);  // this would exit if it fail
        }
    }
    
    /* 5.  Sleep. */
    printf ("Main thread sleeping for %d seconds.\n", run);
    fflush(stdout);
    // main sleep for run time
    sleep(run);
    
    /* 6.  Kill threads and exit.  */
    // killing all threads, and exit
    for (i = 0; i < numProducer; i++)
    {   // killing the producer thread
        pthread_cancel(producerT[i]);
    }
    for (i = 0; i < numConsumer; i++)
    {   // killing the consumer thread
        pthread_cancel(consumerT[i]);
    }
    printf ("Main thread exiting.\n");
    exit (1);
}
