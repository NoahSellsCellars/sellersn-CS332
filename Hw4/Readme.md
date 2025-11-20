# CS332 Homework 4

  * **Author:** Noah Sellers
  * **Email:** sellersn@uab.edu
  * **Date:** November 20, 2025

## Overview

This program is a C-based demonstration of key concurrency concepts, implementing a multi-process, multi-threaded **producer-consumer** solution.

The program's main goal is to:

1.  Spawn 10 **producer threads** in a parent process. Each producer generates 500 random numbers, for a total of 5,000.
2.  Send these 5,000 numbers from the parent process to a child process using a `pipe`.
3.  Spawn 20 **consumer threads** in the child process. Each consumer reads 250 numbers from the pipe and calculates their sum.
4.  The child process then calculates the average of all 20 sums and writes the final result to an `output.txt` file.

-----

## How It Works

The program uses a `fork()` to create two main processes: a parent (for producers) and a child (for consumers). A `pipe()` is established for one-way communication from the parent to the child.

  * **Parent Process (Producers):**

      * Spawns 10 `producer_threads`, each running the `gen_nums` function.
      * Each thread generates 500 random numbers and writes them to the `pipe`.
      * Prints a completion message to the console when done.

  * **Child Process (Consumers):**

      * Spawns 20 `consumer_threads`, each running the `calc_sums` function.
      * Each thread reads 250 numbers from the `pipe` and calculates a local sum.
      * This sum is stored in a shared `sums` array.
      * After all consumers finish, the child process calculates the average of the 20 sums.

  * **Synchronization (The Bounded Buffer):**

      * The `pipe` is treated as a **bounded buffer** (size `BUFFER_SIZE = 100`) to prevent the producers from overflowing the pipe or the consumers from reading from an empty pipe.
      * **Semaphores:** Two semaphores, stored in shared memory (`mmap`), manage the buffer:
          * `open_slots`: Initialized to `BUFFER_SIZE`. Producers `wait()` on this before writing.
          * `full_slots`: Initialized to `0`. Consumers `wait()` on this before reading.
      * **Mutexes:** A `write_mutex` (in the parent) and a `read_mutex` (in the child) protect the pipe from race conditions, ensuring that only one thread can write to or read from the pipe at any given time.

  * **Final Output:**

      * The child process uses `dup2()` to redirect its standard output to a file named `output.txt`.
      * It then `printf()`s the final average, which is written directly into the file.

-----

## Concurrency Topics Used

This project demonstrates practical application of the following CS topics:

  * **Process Management:** Creating and managing a child process using `fork()` and `wait()`.
  * **Inter-Process Communication (IPC):** Using a `pipe()` to send data from the parent process to the child process.
  * **Multithreading:** Creating, running, and joining POSIX threads (`pthread_create()`, `pthread_join()`) for both producers and consumers.
  * **Synchronization Primitives:**
      * **Mutexes** (`pthread_mutex_t`) to ensure atomic access to the pipe (preventing data corruption).
      * **Semaphores** (`sem_t`) to implement a bounded buffer, coordinating producers and consumers.
  * **Shared Memory:** Using `mmap()` to create shared semaphores that can be accessed by both the parent and child processes.
  * **Output Redirection:** Using `fcntl.h` and `dup2()` to redirect the program's final output to a text file.

-----

## How to Run

### 1\. Compile

You can compile the program using the provided `Makefile` or by running the `gcc` command manually.

**Method 1: Using the Makefile (Recommended)**

Simply run the `make` command to build the executable:

```bash
make
```

**Method 2: Compiling Manually (Alternative)**

If you do not use the `Makefile`, you must manually link the POSIX threads library (`-lpthread`):

```bash
gcc -Wall -O -o Hw4 Hw4.c -lpthread
```

### 2\. Run

After compiling, execute the binary:

```bash
./Hw4
```

(You can also use `make run` which compiles and runs the program in one step).

-----

## Expected Output

When you run the program, you will see two things:

1.  **In your terminal:** The completion messages from the 10 producer threads (the order may vary due to thread scheduling).

    ```
    Thread completed (140263654516288)
    Thread completed (140263646123584)
    Thread completed (140263662908992)
    Thread completed (140263637730880)
    Thread completed (140263629338176)
    Thread completed (140263620945472)
    Thread completed (140263612552768)
    Thread completed (140263604160064)
    Thread completed (140263595767360)
    Thread completed (140263587374656)
    ```

2.  **In `output.txt`:** A new file named `output.txt` will be created in the same directory, containing the final calculated average.

    ```
    Average: 1073850123.450000
    ```