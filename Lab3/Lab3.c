#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char ** readStrings(int *count);

/* main method */
int main(void){

	int count;
	char **stringArray = readStrings(&count);

	//sort strings
	char *tempString;
	int currLoc;
	for (int i=1; i < count; i++){
		currLoc = i;
		while (currLoc > 0 && strcmp(stringArray[currLoc-1],stringArray[currLoc]) > 0){
			tempString = stringArray[currLoc];
			stringArray[currLoc] = stringArray[currLoc-1];
			stringArray[currLoc-1] = tempString;
			currLoc--;
		}
	}

	//print the sorted strings
	printf("Sorted strings: \n");
	for (int i = 0; i < count; i++) {
		printf("%s\n", stringArray[i]);
	}

	return 0;
}

char ** readStrings(int *count) {
	//get the number of strings to sort
	printf("Please enter the number of strings to sort (limit 999):\n");
	//if scanf returns more than 1 element, or the count is out of range
	if (scanf("%d", count) != 1 || *count <= 1 || *count > 999) {
		fprintf(stderr, "Invalid input. Please enter a valid number.\n");
		return NULL;
	}
	//clear the input buffer
	while (getchar() != '\n');

	//allocate memory for the array of pointers to the strings
    char** stringArray;
    stringArray = (char **)malloc(sizeof(char *) * *count);
    if (stringArray == NULL) {
        fprintf(stderr, "Memory allocation for string array failed\n");
        return NULL;
    }
	// read the strings
    printf("Please enter %d strings\n", *count);
    for (int i = 0; i < *count; i++) {
        //for each string, store it in a temporary buffer, determine the size,
        //allocate the memory for it, add the pointer to the string to the array of pointers

		//temporary buffer to store the string
        char tempBuffer[256];

		//read the string, if there's nothing there print an error & cleanup
        if(fgets(tempBuffer, sizeof(tempBuffer), stdin) == NULL) {
			fprintf(stderr, "Error reading string\n");
			//clear all the strings
			for (int j = 0; j < i; j++) {
				free(stringArray[j]);
			}
			//clear the string array
			free(stringArray);
			return NULL;
		}
        //change the \n to a \0
        tempBuffer[strcspn(tempBuffer, "\n")] = '\0';

        //determine the length
        int length = strlen(tempBuffer);

		//create a pointer to the string
        char *string;
		//allocate enough memory for the string + the terminating char
        string = (char *)malloc((length + 1) * sizeof(char));
        if (string == NULL) {
            fprintf(stderr, "Allocation for string %d failed!\n", i);
			//clear each string
			for (int j = 0; j < i; j++) {
				free(stringArray[j]);
			}
			//clear the string array
			free(stringArray);
			return NULL;
        }
		//copy the string to the newly allocated memory
        strcpy(string, tempBuffer);

		//add the new string's address to the array of pointers
		stringArray[i] = string;
    }
	return stringArray;
}
