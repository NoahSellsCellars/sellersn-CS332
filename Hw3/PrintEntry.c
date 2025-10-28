/*
This program takes a filepath (absolute or relative) and prints:
1. The file name
2. Size of the file
3. Number of words in the file (only .txt files)

If The path points to a directory, simply print the name of the directory
*/

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>

int wordCount(char* filepath) {
    int wordCount = 0;

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Error Counting words\n");
        return -1;
    }

    int inWord = 0;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (isspace(ch)) {
            inWord = 0;
        } else {
            if (inWord == 0) {
                wordCount++;
                inWord = 1;
            }
        }
    }

    fclose(fp);
    return wordCount;
}

int isDirectory(struct stat statbuf) {
    return S_ISDIR(statbuf.st_mode);
}

char* parseName(char* filepath) {
    char* name = filepath;

    char* lastSlash = strrchr(filepath, '/');
    if (lastSlash != NULL) {
        name = lastSlash + 1;
    }

    return name;
}

int isTxt(char* filepath) {
    char* extention = strrchr(filepath, '.');

    if (extention != NULL && strcmp(extention, ".txt") == 0) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[]) {

    //Check for correct usage
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filepath> <depth>\n", argv[0]);
        printf("%d", argc);
        exit(EXIT_FAILURE);
    }

    //Parse the args
    char* filepath = argv[1];
    int depth = atoi(argv[2]);

    struct stat statbuf;
    char* fileName;
    off_t fileSize;

    //Get the file stats
    if (stat(filepath, &statbuf) != 0) {
        perror("Problem reading file stats\n");
        exit(EXIT_FAILURE);
    }

    //For the indentation
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }

    //These are the same regardless of whether it is a directory
    fileName = parseName(filepath);
    fileSize = statbuf.st_size;

    if (isDirectory(statbuf)) {
        printf("%s (size: %ld bytes)\n", fileName, fileSize);
        return 0;
    }
    else {
        if (isTxt(filepath)) {
            int words = wordCount(filepath);
            if (wordCount < 0) {
                perror("Problem reading words");
                exit(EXIT_FAILURE);
            }
            printf("%s (size: %ld bytes | words: %d)\n", fileName, fileSize, words);
            return 0;
        }
        printf("%s (size: %ld bytes)\n", fileName, fileSize);
        return 0;
    }
}