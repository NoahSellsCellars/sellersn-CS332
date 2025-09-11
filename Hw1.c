#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

int sumOfDigits(int n);
int UABMaxMinDiff(int arr[], int size);
int* replaceEvenWithZero(int arr[], int size);
int perfectSquare(int n);
int countVowels(char s[]);
int arrayMax(int arr[], int size);
int arrayMin(int arr[], int size);
int digitCounter(int n);

int main(void) {

}

int sumOfDigits(int n) {
    //Takes a positive integer and returns
    //the sum of its digits ex. 23 = 2+3 = 5
    if (n < 0) {
        n = -n;
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
    double squareRoot = sqrt(n);
    if(fmod(squareRoot, 1) == 0) {
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