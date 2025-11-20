/****************************************************************************** 
*  Program that makes use of all concurrency topics learned in UAB CS332.     *  
*  This program will generate 5000 random numbers with 10 producer threads.   *
*  After each producer thread finishes, it will print a completion message.   *
*  20 Consumer threads will read a 250 number chunk of the 5000 and sum them. *
*  The program will then calculate the average of each consumer thread's sum  *
*  and dump the result into a text file.                                      *
*                                                                             *
*  Topics Used:                                                               *
*   -Process Management                                                       *
*   -Pipe Operations                                                          *
*   -Child Process and Thread Creation                                        *
*   -Output redirection                                                       *
*                                                                             *
*  To Compile: gcc -Wall -O -o Hw4 Hw4.c -lpthread                            *
*  To run: ./Hw4                                                              *
*                                                                             *
*  Author: Noah Sellers                                                       *
*  Email: sellersn@uab.edu                                                    *
*  Date: November 20, 2025                                                    *
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BUFFER_SIZE 100
#define NUM_PRODUCERS 10
#define NUMS_PER_PRODUCER 500
#define NUM_CONSUMER 20
#define NUMS_PER_CONSUMER 250

// ---Global variables---
sem_t *full_slots;
sem_t *open_slots;

// ---Struct holding arguments for threads---
struct thread_args {
    int thread_id;    //ID of the current thread
    int fd[2];  //Pipe to the parent process
    pthread_mutex_t *mutex; //Mutex for reading/writing to the pipe
    sem_t *open_slots;   //Semaphore for open slots in the pipe (ensures no overflow)
    sem_t *full_slots;     //Semaphore for filled slots in the pipe (ensures no underflow)
    long long *sums_list; //List of sums, length 250
};


void *gen_nums(void *arg) {
    /*Helper function that each producer thread uses
    Takes: void * to thread_args struct
    Does:
    -Generate a random number
    -Send the numbers to the pipe, make sure to call the sem_wait() to wait until there is a free slot and lock the mutex before writing to the pipe
    -Write the number to the pipe
    -After writing, free the mutex and call the sem_post 
    -Repeat 500 times
    -After a thread completes 500 numbers print "Thread completed {threadID}" 
    */

    struct thread_args *thread_args = (struct thread_args*)(arg); //Cast the input back into a thread_args struct

    pthread_t global_id = pthread_self();   //Global thread ID
    pthread_mutex_t *write_mutex = thread_args->mutex;  //Mutex for the pipe
    int write_fd = thread_args->fd[1];   //Write end of pipe to parent
    sem_t *open_slots = thread_args->open_slots; //Semaphores for the pipe
    sem_t *full_slots = thread_args->full_slots;
    int i;
    
    for (i = 0; i < NUMS_PER_PRODUCER; i++) {
        //Generate a number
        int random_num = rand();

        //Write logic
        sem_wait(open_slots);  //Wait until there's an open slot
        pthread_mutex_lock(write_mutex);   //lock
        if (write(write_fd, &random_num, sizeof(int)) == -1) { //Write it to the pipe
            perror("Error writing to pipe");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(write_mutex);     //unlock
        sem_post(full_slots);    //Signal that there's now a full slot
    }

    printf("Thread completed (%ld)\n", global_id);

    free(thread_args);
    return NULL;
}

void *calc_sums(void *arg) {
    /*Helper function that each consumer thread uses
        Takes: void * to thread_args struct
        Does:
        -Call the sem_wait to make sure that there is at least 1 filled slot to read from the pipe and lock the mutex
        -Read the number from the pipe
        -Unlock the mutex and call the sem_post to signal that there is now an empty slot
        -Repeat 250 times and each time add the number to the total sum of all previous iterations
        -After completion write the sum to the sums array[unique threadID]*/
        
        struct thread_args *thread_args = (struct thread_args*)(arg); //Cast the input back into a thread_args struct
        
        int thread_id = thread_args->thread_id; //Unique thread ID (corresponds with order that threads were produced in, ex. 0 for the first thread)
        pthread_mutex_t *read_mutex = thread_args->mutex;  //Mutex for the pipe
        int read_fd = thread_args->fd[0];   //Read end of pipe to parent
        sem_t *open_slots = thread_args->open_slots; //Semaphores for the pipe
        sem_t *full_slots = thread_args->full_slots;
        long long *sums_list = thread_args->sums_list;
        int i;
        
    long long sum = 0;
    for (i = 0; i < NUMS_PER_CONSUMER; i++) {
        int num;
        sem_wait(full_slots);   //Wait until there is at least one slot to read form
        pthread_mutex_lock(read_mutex); //Lock the mutex
        if (read(read_fd, &num, sizeof(int)) == -1) {   //Read from the pipe
            perror("Error reading from pipe");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(read_mutex);   //Unlock the mutex
        sem_post(open_slots);   //Signal to the producer threads that there is now a free slot
        sum += num;
    }

    sums_list[thread_id] = sum;

    free(thread_args);
    return NULL;
}

int main(int argc, char* argv[]){
    srand(time(NULL) * getpid());   //Seed the random function that will be used later

    int pipe_fd[2]; //Pipe

    //Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe\n");
        exit(EXIT_FAILURE);
    }
    //Allocate shared memory for semaphores
    void *shm = mmap(NULL, 2 * sizeof(sem_t), 
                     PROT_READ | PROT_WRITE, 
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    //Assign the semaphore names
    open_slots = (sem_t*)shm;
    full_slots = (sem_t*)(shm + sizeof(sem_t));

    //Create the full/empty slots sempahores (empty is for making sure we don't overflow and full is making sure we don't underflow)
    if (sem_init(open_slots, 1, BUFFER_SIZE) != 0) {
        perror("sem_init open_slots\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(full_slots, 1, 0) != 0) {
        perror("sem_init full_slots\n");
        exit(EXIT_FAILURE);
    }

    //Create a child process
    pid_t pid = fork();

    if (pid > 0) { //This is the parent process

        close(pipe_fd[0]); //Close the read end of the pipe

        //Create the write mutex
        pthread_mutex_t write_mutex; 
        pthread_mutex_init(&write_mutex, NULL); 
        //Create ten producer threads
        int i;
        pthread_t producer_threads[10];
        for (i = 0; i < NUM_PRODUCERS; i++) {
            //Create and populate the struct for each thread
            struct thread_args *thread_args = malloc(sizeof(struct thread_args));
            thread_args->thread_id = i;
            thread_args->fd[1] = pipe_fd[1];
            thread_args->mutex = &write_mutex;
            thread_args->open_slots = open_slots;
            thread_args->full_slots = full_slots;
            //Create the thread
            pthread_create(&producer_threads[i], NULL, gen_nums, (void*)(thread_args));
        }

        //Wait for every thread to finish before moving on
        for (i = 0; i < NUM_PRODUCERS; i++) {
            pthread_join(producer_threads[i], NULL);
        }

        close(pipe_fd[1]); //Close the write end of the pipe

        wait(NULL); //Wait on the child process
        
        //Clean up
        pthread_mutex_destroy(&write_mutex);    
        sem_destroy(full_slots);
        sem_destroy(open_slots);
    }
    
    else if (pid == 0) { //This is the child process
        close(pipe_fd[1]); //Close the write end of the pipe

        //Create the read mutex
        pthread_mutex_t read_mutex; 
        pthread_mutex_init(&read_mutex, NULL); 

        //Create an array for the sums each thread will return
        long long sums[NUM_CONSUMER];
        //Create 20 consumer threads
        pthread_t consumer_threads[NUM_CONSUMER];
        
        int i;
        for (i = 0; i < NUM_CONSUMER; i++) {
            struct thread_args *thread_args = malloc(sizeof(struct thread_args));
            thread_args->thread_id = i;
            thread_args->fd[0] = pipe_fd[0];
            thread_args->mutex = &read_mutex;
            thread_args->open_slots = open_slots;
            thread_args->full_slots = full_slots;
            thread_args->sums_list = sums;
            pthread_create(&consumer_threads[i], NULL, calc_sums, (void*)(thread_args));
        }
        //Wait for all threads to complete
        for (i = 0; i < NUM_CONSUMER; i++) {
            pthread_join(consumer_threads[i], NULL);
        }

        close(pipe_fd[0]); //Close the read end of the pipe

        //Calculate the average of all the sums
        double sums_avg_numerator = 0;
        int sums_avg_denom = NUM_CONSUMER;
        double average;

        for (int i = 0; i < NUM_CONSUMER; i++) {
            sums_avg_numerator += sums[i];
        }
        average = sums_avg_numerator / (double)sums_avg_denom;
        //Create the output file
        int file_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0664);
        if (file_fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        //Redirect stdout to a text file
        if (dup2(file_fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        //Print the average to the text file
        printf("Average: %f\n", average);

        pthread_mutex_destroy(&read_mutex); //Clean up
        return 0;
    }

}