#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int sumOfDigits(int n);
int UABMaxMinDiff(int arr[], int size);
int* replaceEvenWithZero(int arr[], int size);
int perfectSquare(int n);
int countVowels(char s[]);
int arrayMax(int arr[], int size);
int arrayMin(int arr[], int size);

int main(void) {
    //test functions
    printf("--- Testing sumOfDigits ---\n");
    printf("Input: n = 123, Expected: 6, Output: %d\n", sumOfDigits(123));
    printf("Input: n = 405, Expected: 9, Output: %d\n", sumOfDigits(405));
    printf("Input: n = 0, Expected: -1, Output: %d\n", sumOfDigits(0));
    printf("Input: n = 7, Expected: 7, Output: %d\n", sumOfDigits(7));
    printf("Input: n = -308, Expected: -1, Output: %d\n\n", sumOfDigits(-308));

    printf("--- Testing UABMaxMinDiff ---\n");
    int arr1[] = {3, 7, 2, 9};
    int arr2[] = {5, 5, 5, 5, 5, 5, 5};
    int arr3[] = {-2, 4, -1, 6, 5};
    printf("Input: arr = [3,7,2,9], Expected: 7, Output: %d\n", UABMaxMinDiff(arr1, 4));
    printf("Input: arr = [5,5,5,5,5,5,5], Expected: 0, Output: %d\n", UABMaxMinDiff(arr2, 7));
    printf("Input: arr = [-2,4,-1,6,5], Expected: 8, Output: %d\n\n", UABMaxMinDiff(arr3, 5));

    printf("--- Testing replaceEvenWithZero ---\n");
    int arr4[] = {1, 2, 3, 4};
    int arr5[] = {2, 4, 6};
    int arr6[] = {1, 3, 5};
    int *result4 = replaceEvenWithZero(arr4, 4);
    int *result5 = replaceEvenWithZero(arr5, 3);
    int *result6 = replaceEvenWithZero(arr6, 3);
    printf("Input: arr = [1,2,3,4], Expected: [1, 0, 3, 0] Output: [");
    for (int i = 0; i < 4; i++) {
        if (result4 != NULL) {
            printf("%d", result4[i]);
            if (i < 3) printf(", ");
        }
    }
    printf("]\n");
    printf("Input: arr = [2,4,6], Expected: [0, 0, 0] Output: [");
    for (int i = 0; i < 3; i++) {
        if (result5 != NULL) {
            printf("%d", result5[i]);
            if (i < 2) printf(", ");
        }
    }
    printf("]\n");
    printf("Input: arr = [1,3,5], Expected: [1, 3, 5] Output: [");
     for (int i = 0; i < 3; i++) {
        if (result6 != NULL) {
            printf("%d", result6[i]);
            if (i < 2) printf(", ");
        }
    }
    printf("]\n");
    if (result4 != NULL) free(result4);
    if (result5 != NULL) free(result5);
    if (result6 != NULL) free(result6);

    printf("--- Testing perfectSquare ---\n");
    printf("Input: n = 16, Expected: True, Output: %s\n", perfectSquare(16) ? "True" : "False");
    printf("Input: n = 15, Expected: False, Output: %s\n", perfectSquare(15) ? "True" : "False");
    printf("Input: n = 25, Expected: True, Output: %s\n", perfectSquare(25) ? "True" : "False");
    printf("Input: n = 36, Expected: True, Output: %s\n\n", perfectSquare(36) ? "True" : "False");

    printf("--- Testing countVowels ---\n");
    char s1[] = "Hello World";
    char s2[] = "UAB CS";
    char s3[] = "Python";
    char s4[] = "aeiou";
    printf("Input: s = \"Hello World\", Expected: 3, Output: %d\n", countVowels(s1));
    printf("Input: s = \"UAB CS\", Expected: 2, Output: %d\n", countVowels(s2));
    printf("Input: s = \"Python\", Expected: 1, Output: %d\n", countVowels(s3));
    printf("Input: s = \"aeiou\", Expected: 5, Output: %d\n\n", countVowels(s4));

    return 0;
}

int sumOfDigits(int n) {
    //Takes a positive integer and returns
    //the sum of its digits ex. 23 = 2+3 = 5
    if (n <= 0) {
        return -1;
    }
    int sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

int UABMaxMinDiff(int arr[], int size) {
    //Takes an array of integers and returns
    //the difference between the max and min
    return arrayMax(arr, size) - arrayMin(arr, size);
}

int* replaceEvenWithZero(int arr[], int size) {
    //Takes an array of integers and returns
    //a pointer to a new array with all of
    //the even number replaced with 0
    int* result = (int*)malloc(size*sizeof(int));
    if (result == NULL) {
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) {
            result[i] = 0;
        }
        else {
            result[i] = arr[i];
        }
    }
    return result;
}

int perfectSquare(int n) {
    //Takes an integer n and returns True
    //if that number is a perfect square
    if (n < 0)
    {
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    double squareRoot = sqrt((double)n);
    int intSquareRoot = (int)squareRoot;
    if (intSquareRoot * intSquareRoot == n) {
        return 1;
    }
    else {
        return 0;
    }

}

int countVowels(char s[]) {
    //Takes a string and returns the number
    //of vowels in the string ignoring case
    int vowelCounter = 0;
    for (int i = 0; i < strlen(s); i++) {
        char c = tolower(s[i]);
        if (c == 'a' ||
            c == 'e' ||
            c == 'i' ||
            c == 'o' ||
            c == 'u') {
            vowelCounter++;
        }
    }
    return vowelCounter;
}

int arrayMax(int arr[], int size) {
    //Helper function to find max in an array
    if (size <= 0) {
        return 0;
    }
    int max = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

int arrayMin(int arr[], int size) {
    //Helper function to find min in an array
    if (size <= 0) {
    return 0;
    }
    int min = arr[0];
    for (int i = 1; i < size; i++) {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}