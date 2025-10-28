#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_PATH_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage %s <path/to/starting/directory>\n", argv[0]);
        return -1;
    }

    char* basepath = argv[1];
    DIR* dirPtr;
    struct dirent* entry;

    if ((dirPtr = opendir(basepath)) == NULL) {
        perror("opendir");
        return -1;
    }

    
    char* depthStr = "0";

    //Print the directory itself
    pid_t pid = fork();
    if (pid == 0) {
        execl("./PrintEntry", "PrintEntry", basepath, depthStr, NULL);
    }
    else {
        wait(NULL);
    }

    //Print its contents

    depthStr = "1";

    while ((entry = readdir(dirPtr)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }

        //Build the path to the file
        char filepath[MAX_PATH_SIZE];
        snprintf(filepath, MAX_PATH_SIZE, "%s/%s", basepath, entry->d_name);

        int pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(-1);
        }
        if (pid == 0) { //This is the child
            execl("./PrintEntry", "PrintEntry", filepath, depthStr, NULL);
        }
        else {
            wait(NULL);
        }
    }
    closedir(dirPtr);
    return 0;
}