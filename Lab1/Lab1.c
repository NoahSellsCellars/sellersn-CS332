/*
Compile and Run Instructions:

1. Open a terminal
2. Execute the follwing command to compile the program:
    gcc Lab1.c -o Lab1
3. To run the program execute:
    ./Lab1
4. The program will prompt for the user to enter an integer.
*/

#include <stdio.h>

int isPrime(int num);

int main(void) {
    int given_number;
    printf("Please enter an integer\n");
    scanf("%d", &given_number);
    if (isPrime(given_number)) {
        printf("The number is prime\n");
    }
    else {
        printf("The number is not prime\n");
    }
    return 0;

}

int isPrime(int num) {
    //Takes an integer and returns true if the integer is a prime number
    if (num <= 1) {
        return 0;
    }
    else if(num == 2 || num == 3 ) {
        return 1;
    }
    else if (num % 2 == 0 || num % 3 == 0) {
        return 0;
    }
    int i = 5;
    while (i * i <= num) {
        if (num % i == 0 || num % (i+2) == 0) {
            return 0;
        }
        i += 6;
    }
    return 1;
}