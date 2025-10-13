/****************************************************************************** 
*                                                                             *
*  To Compile: gcc -Wall -O -o lab7 lab7.c                                    *
*  To run: ./lab7 <instruction file>                                          *
*                                                                             *
*  Author: Noah Sellers                                                       *
*  Email: sellersn@uab.edu                                                    *
*  Date: October 12, 2025                                                     *
******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char** argv) {
    char *file_path = argv[1];
    FILE *instruction_file;
    FILE *log_file;

    instruction_file = fopen(file_path, "r");
    if (instruction_file == NULL) {
        fprintf(stderr, "Problem opening file");
        exit(-1);
    }

    log_file = fopen("log", "w");
     if (log_file == NULL) {
        fprintf(stderr, "Problem opening file");
        exit(-1);
    }
    int LINESIZE_CHARS = 1024;
    char line[LINESIZE_CHARS];

    while (fgets(line, LINESIZE_CHARS, instruction_file) != NULL) {
        line[strcspn(line, "\n")] = 0;

        char *args[64];
        char *token;
        int i = 0;
        
        char line_copy[LINESIZE_CHARS];
        strncpy(line_copy, line, LINESIZE_CHARS);

        token = strtok(line_copy, " ");
        while (token != NULL) {
            args[i] = token;
            i++;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        time_t start_time = time(NULL);
        time_t end_time;

        pid_t pid;
        int status;

        pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(-1);
        }

        else if (pid == 0) {
            execvp(args[0], args);
        }

        else {
            wait(NULL);
            end_time = time(NULL);
        }

        char start_time_string[30];
        char end_time_string[30];

        strcpy(start_time_string, ctime(&start_time));
        strcpy(end_time_string, ctime(&end_time));

        start_time_string[strcspn(start_time_string, "\n")] = 0;
        end_time_string[strcspn(end_time_string, "\n")] = 0;

        fprintf(log_file, "%s\t%s\t%s\n", line, start_time_string, end_time_string);
    }

    fclose(instruction_file);
    fclose(log_file);
}