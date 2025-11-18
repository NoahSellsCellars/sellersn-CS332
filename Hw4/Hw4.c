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
    pthread_mutex_t *mutex;
    sem_t *open_slots;   //Semaphore for open slots in the pipe
    sem_t *full_slots;     //Semaphore for filled slots in the pipe
    long long *sums_list; //List of sums, length 250
};


void *gen_nums(void *arg) {
    /*In the thread:
    Input is a struct with threadid, semaphores, and pipe adress
    Generate 500 random numbers
    Send the numbers to the pipe, make sure to lock the mutex and call the sem_wait() until there is a free slot before writing
    After writing call the sem_post and free the mutex
    print "Thread completed {threadID}" */

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
        write(write_fd, &random_num, sizeof(int));  //Write it to the pipe
        pthread_mutex_unlock(write_mutex);     //unlock
        sem_post(full_slots);    //Signal that there's now an empty slot
    }

    printf("Thread completed (%ld)\n", global_id);

    free(thread_args);
    return NULL;
}

void *calc_sums(void *arg) {
    /*In each thread:
        Read 250 numbers from the pipe (for this use a while loop and grab one int at a time adding it to the sum)
            make sure to lock the mutex and call the sem_wait() for full slots before reading
        After writing call the sem_post and free the mutex
        Calculate the sum of the 250 numbers and write the sum to the sums array[unique id]*/
    long long sum = 0;

    struct thread_args *thread_args = (struct thread_args*)(arg); //Cast the input back into a thread_args struct

    pthread_t global_id = pthread_self();   //Global thread ID
    int thread_id = thread_args->thread_id;
    pthread_mutex_t *read_mutex = thread_args->mutex;  //Mutex for the pipe
    int read_fd = thread_args->fd[0];   //Read end of pipe to parent
    sem_t *open_slots = thread_args->open_slots; //Semaphores for the pipe
    sem_t *full_slots = thread_args->full_slots;
    long long *sums_list = thread_args->sums_list;
    int i;

    for (i = 0; i < NUMS_PER_CONSUMER; i++) {
        int num;
        sem_wait(full_slots);
        pthread_mutex_lock(read_mutex);
        read(read_fd, &num, sizeof(int));
        pthread_mutex_unlock(read_mutex);
        sem_post(open_slots);
        sum += num;
    }

    sums_list[thread_id] = sum;

    free(thread_args);
    return NULL;
}

int main(int argc, char* argv[]){
    srand(time(NULL) * getpid());

    int pipe_fd[2]; //Pipe

    //Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe\n");
        exit(EXIT_FAILURE);
    }
    //Allocate memory for sems
    void *shm = mmap(NULL, 2 * sizeof(sem_t), 
                     PROT_READ | PROT_WRITE, 
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

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

        for (i = 0; i < NUM_PRODUCERS; i++) {
            pthread_join(producer_threads[i], NULL);
        }

        close(pipe_fd[1]); //Close the write end of the pipe

        wait(NULL);
        pthread_mutex_destroy(&write_mutex);
        sem_destroy(full_slots);
        sem_destroy(open_slots);
    }
    
    else if (pid == 0) { //This is the child process
        /*In the the child process:
            Create twenty consumer threads (pass a unique id from 0-19 to each thread)*/
            close(pipe_fd[1]); //Close the write end of the pipe

            //Create the read mutex
            pthread_mutex_t read_mutex; 
            pthread_mutex_init(&read_mutex, NULL); 

            //Create an array for the sums each thread will return
            int i;
            long long sums[NUM_CONSUMER];
            //Create 20 consumer threads
            pthread_t consumer_threads[NUM_CONSUMER];
            
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
            //Print the average(Will go to text file)
            printf("Average: %f\n", average);

            pthread_mutex_destroy(&read_mutex);
            return 0;
    }

}