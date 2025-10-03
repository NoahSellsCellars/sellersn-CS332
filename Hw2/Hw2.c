#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>

struct options{
    int S_flag;

    int s_flag;
    int file_size;

    int f_flag;
    char* substring;
    int depth;
};

typedef void PRINT_FUNC(char* filepath, struct options options, struct stat statbuf, int* depth);

void print_dir_contents(char* filepath, struct options options, int* depth);
void print_detailed_entry(char* filepath, struct options options, struct stat statbuf, int* depth);
void print_simple_entry(char* filepath, struct options options, struct stat statbuf, int* depth);
void print_detail(char* filepath, struct options options, struct stat statbuf, int* depth, PRINT_FUNC *f);

int main(int argc, char** argv) {
    struct options options = {0};
    int depth_value = 0;
    int* depth = &depth_value;

    //Parse the options
    int option;
    const char* optstring = "Ss:f:";
    while ((option = getopt(argc, argv, optstring)) != -1) {
        switch(option) {
            case 'S':
                options.S_flag = 1;
                break;
            case 's':
                options.s_flag = 1;
                options.file_size = atoi(optarg);
                break;
            case 'f':
                options.f_flag = 1;
                options.substring = strdup(optarg);
                break;
            case '?':
                printf("Usage: %s [<starting directory>] [-S] [-s <size(bytes)>] [-f <string option> <depth>] \n", argv[0]);
                exit(-1);
        }
        
    }

    //Check the usage
    int S_num_args = options.S_flag ? 1 : 0;
    int s_num_args = options.s_flag ? 2 : 0;
    int f_num_args = options.f_flag ? 3 : 0;
    int total_args = S_num_args + s_num_args + f_num_args + 2; // +1 for ./program and +1 for optional starting_dir arg

    //printf("%d out of %d", total_args, argc);
    if(!((argc == total_args -1) || argc == total_args)) {
        printf("Usage: %s [<starting directory>] [-S] [-s <size(bytes)>] [-f <string option> <depth>] \n", argv[0]);
        exit(-1);
    }
    //Parse the starting directory
    char* starting_dir = ".";

    if (optind < argc) {
        //If there are 2 more non-option args
        if (options.f_flag && optind + 1 < argc) {
            options.depth = atoi(argv[optind + 1]);
            starting_dir = (argv[optind]);
        }
        else {
            if(options.f_flag) {
                options.depth = atoi(argv[optind]);
            }
            else{
                starting_dir = argv[optind];
            }
        }
    }


    //Print the contents of the directory, including the contents of its subdirectories
    print_dir_contents(starting_dir, options, depth);
    if (options.substring != NULL) {
        free(options.substring);
    }
}

void print_dir_contents(char* filepath, struct options options, int* depth) {
    struct dirent* dirent;
    struct stat statbuf;
    DIR* dir;
    
    //Get the stats of the directory
    if(lstat(filepath, &statbuf) < 0) {
        printf("Problem reading directory \"%s\". Ensure path is valid directory", filepath);
        exit(-1);
    }

    //Only print the directory if the depth and size are correct
    if (((options.f_flag && *depth <= options.depth) || options.f_flag != 1) && 
        ((options.s_flag && (statbuf.st_size < options.file_size)) || options.s_flag != 1)) {
        //Print the basename of the directory and, if required, its info
        if (options.S_flag) {
            print_detail(filepath, options, statbuf, depth, print_detailed_entry);
        }
        else {
            print_detail(filepath, options, statbuf, depth, print_simple_entry);
        }
    }

    //Open the directory
    if((dir = opendir(filepath)) == NULL) {
        printf("Problem opening directory %s", filepath);
        exit(-1);
    }

    //Increase the depth
    (*depth)++;

    //Loop through each entry and print it
    while ((dirent = readdir(dir)) != NULL) {
        //Ignore the "." and ".." entries
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) {
            continue;
        }

        //Build the path to the entry
        char* entry_path = malloc(strlen(filepath) + strlen("/") + strlen(dirent->d_name) + 1);
        entry_path = strcpy(entry_path, filepath);
        strcat(entry_path, "/");
        strcat(entry_path, dirent->d_name);

        //Get the stats of the entry
        if(lstat(entry_path, &statbuf) < 0) {
            printf("Problem reading directory stats");
    }

        //If the entry is a directory, call print_dir_contents then continue
        if (S_ISDIR(statbuf.st_mode)) {
            print_dir_contents(entry_path, options, depth);
            free(entry_path);
            continue;
        }

        //Make sure that entry meets conditions
        //Check if the file size is too large
        if(options.s_flag && statbuf.st_size > options.file_size) {
            free(entry_path);
            continue;
        }
        //Check if the entry doesn't contain the substring
        if(options.f_flag && (strstr(dirent->d_name, options.substring) == NULL)) {
            free(entry_path);
            continue;
        }
        //Check if the depth is too deep
        if(options.f_flag && *depth > options.depth) {
            free(entry_path);
            continue;
        }

        if (options.S_flag) {
            print_detailed_entry(entry_path, options, statbuf, depth);
        }
        else {
            print_simple_entry(entry_path, options, statbuf, depth);
        }
        free(entry_path);
    }
    closedir(dir);

    //Decrease the depth
    (*depth)--;
}

void print_detailed_entry(char* filepath, struct options options, struct stat statbuf, int* depth) {
    char* path_copy = strdup(filepath);
    char* base_name = basename(path_copy);

    //For indentation
    for (int i = 0; i < *depth; i++) {
        printf("    ");
    }

    //Print the name
    printf("%s ", base_name);

    //If it's a symlink print where it goes to
    if (S_ISLNK(statbuf.st_mode)) {
        ssize_t bytes_read;
        char* target = malloc(statbuf.st_size + 1);
        if (target == NULL) {
            printf("Problem allocating target buffer");
            exit(-1);
        }
        if((bytes_read = readlink(filepath, target, statbuf.st_size)) < 0) {
            printf("Problem reading symlink");
            free(target);
            exit(-1);
        }
        //Add the null terminator
        target[bytes_read] = '\0';
        char* target_copy = strdup(target);
        char* target_base_name = basename(target_copy);

        printf("(%s) ", target_base_name);
        free(target);
        free(target_copy);
    }
    /*STATS SECTION | (size, permissions, last access time)*/
    off_t size;
    char permissions[10];
    time_t access_time_sec;
    char time_str[100];

    //Determine the size
    if (S_ISDIR(statbuf.st_mode)) {
        size = 0;
    }
    else {
        size = statbuf.st_size;
    }

    //Determine the permissions
    //Owner permissions
    permissions[0] = (statbuf.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (statbuf.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (statbuf.st_mode & S_IXUSR) ? 'x' : '-';
    // Group permissions
    permissions[3] = (statbuf.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (statbuf.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (statbuf.st_mode & S_IXGRP) ? 'x' : '-';
    // Others permissions
    permissions[6] = (statbuf.st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (statbuf.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (statbuf.st_mode & S_IXOTH) ? 'x' : '-';
    //Add the null terminator for printing
    permissions[9] = '\0';

    //Determine time of last access and format it from seconds since UNIX epoch to year-month-day hour-minute-second
    access_time_sec = statbuf.st_atime;
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&access_time_sec));

    //Print the stats
    printf("(size: %ld bytes | permissions: %s | last access: %s)", size, permissions, time_str);

    //Print a new line for the next entry
    printf("\n");

    free(path_copy);
}

void print_simple_entry(char* filepath, struct options options, struct stat statbuf, int* depth) {
    //For indentation
    char* path_copy = strdup(filepath);
    char* base_name = basename(path_copy);
    for (int i = 0; i < *depth; i++) {
        printf("    ");
    }

    //Print the name
    printf("%s ", base_name);

    //If it's a symlink print where it goes to
    if (S_ISLNK(statbuf.st_mode)) {
        ssize_t bytes_read;
        char* target = malloc(statbuf.st_size + 1);
        if (target == NULL) {
            printf("Problem allocating target buffer");
            exit(-1);
        }
        if((bytes_read = readlink(filepath, target, statbuf.st_size)) < 0) {
            printf("Problem reading symlink");
            free(target);
            exit(-1);
        }
        //Add the null terminator
        target[bytes_read] = '\0';
        char* target_copy = strdup(target);
        char* target_base_name = basename(target_copy);

        printf("(%s) ", target_base_name);
        free(target);
        free(target_copy);
    }
    //Print a new line for the next entry
    printf("\n");

    free(path_copy);
}

void print_detail(char* filepath, struct options options, struct stat statbuf, int* depth, PRINT_FUNC *f) {
    f(filepath, options, statbuf, depth);
}

