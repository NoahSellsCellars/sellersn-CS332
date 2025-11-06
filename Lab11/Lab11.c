#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

char *string_to_lowercase(char *str) {
    char* str_lower = strdup(str);
    for (int i = 0; str[i] != '\0'; i++) {
        str_lower[i] = tolower(str[i]);
    }
    return str_lower;
}

int main(int argc, char* argv[]) {
    //Start the user input loop
    int time_To_Quit = 0;
    while(!time_To_Quit) {

        //Prompt the user
        printf("Enter a command:\n");

        //Read the user input and check if it says quit
        char command[1024];
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }
        //Remove the newline
        command[strcspn(command, "\n")] = '\0';

        //If the command was quit, it's time to quit
        char* lower_command = string_to_lowercase(command);
        if (strcmp(lower_command, "quit") == 0) {
            time_To_Quit = 1;
            free(lower_command);
            printf("Exiting...\n");
            break;
        }
        free(lower_command);
        //Otherwise, we can go ahead and execute the command with popen
        FILE* fp;
        if ((fp = popen(command, "w")) == NULL) {
            perror("exec");
            exit(EXIT_FAILURE);
        }
        pclose(fp);
    }
}