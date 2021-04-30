# Pthread
Objectives:  Write a program which implements a multiple producer - multiple consumer environment.

Details:

You will be implementing a version of the producer-consumer project from our text using the Pthreads API.  The project is described on pages 253-257 of the 9th edition or pages 274-278 of the 8th edition.  I'll summarize some of the description here as well as add some additional requirements.

Main program:

Your main program will spawn off some number of producer threads and some number of consumer threads, all of which will interact through a single shared buffer.

Here is the skeleton for main():

int main(int argc, char *argv[]) {
   /* 1. Get command line arguments */
   /* 2. Initialize buffer */
   /* 3. Create producer threads. */
   /* 4. Create consumer threads.  */
   /* 5.  Sleep. */
   /* 6.  Kill threads and exit.  */
}

Main will take 3 arguments:
1. The length of time the program should run.
2. The number of producer threads.
3. The number of consumer threads.

When initializing the buffer, assume that the producer and consumer threads will access the shared buffer as shown in Figure 5.9 and 5.10 pn pages 219-220 of the 9th edition or Figures 6.10 and 6.11 on page 240 of the 8th edition.  This solution used one mutex semaphore and 2 counting semaphores, full and empty.  In addition, assume that the threads track the current state of the shared buffer using the in and out pointers as described on page 204 (9th edition) or page 226 (8th edition).  Note that you will use semaphores rather than busy-waiting. 

When creating the threads, you can use the pthread functions detailed in Figure 4.9 on page 174 (9th edition) or page 161 (8th edition).  You will need to write a producer and a consumer function for the threads to begin execution in.  We'll cover this below.

Once the main program has created all the threads, it will sleep for the number of seconds specified on the command line.

Finally, when the main program wakes back up, it should exit (which will kill all the other threads too). 

Producer and consumer functions

Both the producer and consumer threads need to sleep for a random time, then either produce or consume 1 item, and repeat.
From your text, here is a skeleton for the producer function:

#include <stdlib.h>  /* required for rand() */

#include “buffer.h”


void *producer(void *param) {
   buffer_item item;
  
   while(TRUE) {
      /* generate random number */
      /* sleep for random amount of time */
      sleep(...);
      /* generate a random number - this is the producer's product */
      item = rand();
      printf("producer produced %d\n", item);
      if (insert_item(item) < 0) {
         printf("Producer error\n");
      }
   }
}

rand() returns a pseudo-random integer value between 0 and RAND-MAX.  We will use rand() to generate the sleep time and again to generate the product value.
You must have your producer and consumer threads sleep for either 1, 2, or 3 seconds, where the sleep time is uniformly distributed.  You can generate a random number from 1 to 3 by getting a random number from rand() (which returns a number from 0 to RAND_MAX),  then mod by 3 and add 1.  In any case, you pass this number to sleep().

Once the producer thread has slept, you generate another random number from 1-100 (by modding by 100 and adding 1), and insert it into the buffer.  Once the consumer has slept, it attempts to remove an item from the buffer.  Then both repeat.

Inserting and Removing items

All synchronization should be handled inside the insert_item() and remove_item() functions.  So all the interesting stuff using sempahores, mutexes, and in and out will occur here.  Again, use the code from pages described above.  The code must be written so that any number of producers and consumers can access the shared buffer without problems.  The project description shows how to create, initialize, and use semaphores and mutexes in the pthreads model.  Remember to initialize full and empty as described in the book.

Output:

Main program:
When beginning execution, print a message "Main thread beginning".
For each producer thread created, print a message "Creating producer thread with id x" where x is the thread id.  The thread id is returned when you create a thread.
For each consumer thread created, print a message "Creating consumer thread with id x" where x is the thread id.
Before the call to sleep, print "Main thread sleeping for x seconds" where x is the runtime variable from the command line.
Before the main thread exits, print "Main thread exiting".

Producer threads:
Each time a producer thread sleeps, print a message "Producer thread x sleeping for y seconds" where x is the thread id and y is the number of seconds.
Each time a producer thread inserts an item, print a message" "Producer thread x inserted value y" where x is the thread id, and y is the random value product.

Consumer threads:
Each time a consumer thread sleeps, print a message "Consumer thread x sleeping for y seconds" where x is the thread id and y is the number of seconds.
Each time a consumer thread removes an item, print a message" "Consumer thread x removed value y" where x is the thread id, and y is the random value product.

Insert function:
Each time insert_item inserts an item, print "Insert_item inserted item x at position y" where x is the item value and y is the index into the buffer.  Then print the contents of the buffer as shown here.  Print “empty” if the buffer entry is empty or the number stored in the entry.  Lastly print the value of in and out.  (Remember that the indices start at 0.)

Ex.

Insert_item inserted item 27 at position 3
[empty][5][94][27][empty]  in = 4 , out = 1

Remove function:
Each time remove_item removes an item, print "Remove_item removed item x at position y" where x is the item value and y is the index into the buffer. Then print the contents of the buffer and the values for in and out.

Ex.

Remove_item removed item 5 at position 1
[empty][empty][94][27][empty]  in = 4 , out = 2
All this output should help you debug your program.
You will then run your program, first with only 1 producer and 1 consumer, and then with different numbers of producers and consumers and hand in the output.

Notes:

If you are unfamiliar with C function calls, here is an excellent link to on-line man pages, specifically for Linux.  http://www.die.net/doc/linux/man/ 
Here is a nice tutorial on Pthreads from Lawrence Livermore Labs: https://computing.llnl.gov/tutorials/pthreads/

It's always a good idea to use a makefile.  You put all your file dependencies in the file and type "make" on the command line to build the project.  Here is an example called "makefile":
# This is a comment.  OS Program 3
# The 2nd line below must begin with a tab.
kbprog3: kbprog3.c
    gcc -Wall kprog3.c -lpthread -o kbprog3

Note that the thread ID returned from pthread_create() is an unsigned long.   You can printf this value with a "%lu" format.
You may need to link in the pthreads library when compiling. Add "-lpthread" to your compile statement as shown in the makefile above.


Requirements:
You must write your program in C or C++ and run it on the Linux machines.
Your program must be well-commented including function headers and explanations of complex code.  Proper indentation is expected.

To hand in:

A printout of your commented code
A printout of your typescript file
A paragraph in which you explain your results.  In particular, find the throughput of each run, in other words, how many items were produced and then consumed in each case.  Was the throughput of the second case 5 times as large as the first case?  Why or why not?  (You can find the throughput by looking at the output - no coding is necessary).
How to generate typescript file:

1. Log on to you Linux account.
2. Start up the script command. "script"
3. Run your program for 25 seconds with 1 producer and 1 consumer. "kbprog3 25 1 1"
4. Run your program for 25 seconds with 5 producers and 2 consumers. "kbprog3 25 5 2"
5. Exit from script program. "exit"

The resulting script file is called "typescript".
