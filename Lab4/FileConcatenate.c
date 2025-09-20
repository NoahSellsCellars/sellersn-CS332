#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFSIZE 4096

int main(int argc, char *argv[]) {
    //File Descriptors
    int firstFileDescriptor, secondFileDescriptor;
    //Read/Write Bytes
    long int n;
    //Create the buffer to store the copy
    char buf[BUFFSIZE];

    //Check for correct usage
    if (argc != 3) {
        printf("Usage: %s <source_filename> <destination_filename\n", argv[0]);
        
        exit (-1);
    }
    if (strcmp(argv[1],argv[2]) == 0) {
        printf("Error! Cannot concatenate a file with itself!\n");

        exit(-1);
    }

    //Open the first file, if it doesn't exist, create a new file, read-write.
    firstFileDescriptor = open(argv[1], O_CREAT|O_RDWR);
    
    //Open the second file in read-only
    secondFileDescriptor = open(argv[2], O_RDONLY);
    if (secondFileDescriptor == -1) {
        //Check for problems opening the file
        printf("Unable to locate %s!\n", argv[2]);

        exit(-1);
    }

    //Store the contents of the second file in the buffer
    n = read(secondFileDescriptor, buf, BUFFSIZE);

    //Check for problems reading from the file
    if (n < 0) {
        printf("Error with reading the second file");

        exit(-1);
    }

    //Set the cursor in the first file to the end
    lseek(firstFileDescriptor, 0, SEEK_END);

    //Write what's in the buffer to the first file
    if (write(firstFileDescriptor, buf, n) != n) {
        //check to make sure the expected number of characters were written
        printf("Error writing to output file\n");
        
        exit(-1);
        }

    //Close the files
    close(firstFileDescriptor);
    close(secondFileDescriptor);

    return 0;
}