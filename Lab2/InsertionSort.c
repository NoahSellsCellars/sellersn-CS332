#include <stdio.h>
#include<stdlib.h>
#include<time.h>

void insertionSort(int arr[], int length);

int main(void) {
    int n;
    printf("Please enter size of array to sort\n");
    scanf("%d", &n);
    printf("---------------------------------------------\n");
    int arr [n];
    //Populate the array
    for (int i = 0; i < n; i++) {
        printf("Please enter element %d (integer) of the array\n", i+1);
        scanf("%d", &arr[i]);
    }
    printf("---------------------------------------------\n");
    //Print the unsorted elements
    printf("Elements of the array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d\n", arr[i]);
    }
    printf("---------------------------------------------\n");
    //Sort the array
    insertionSort(arr, n);
    //Print the sorted Elements
    printf("Sorted Elements of the array:\n");
    for (int i = 0; i < n; i++) {
        printf("%d\n", arr[i]);
    }
}

void insertionSort(int arr[], int length) {
    //loop through the elements, starting with the second
    for (int j = 1; j < length; j++) {
        //the value we're trying to sort
        int key = arr[j];
        //pointer to the element before
        int i = j - 1;
        //while there is still an element to the left
        //and the element before is larger
        while (i >= 0 && arr[i] > key) {
            //move the element before to the right
            arr[i+1] = arr[i];
            //shift the pointer 1 left, this is the new "element before"
            //i+1 is the position that the pointer was just in
            i--;
        }
        //after there are no more elements before or the subarray is sorted
        //insert the key into the spot that was just moved to the right
        arr[i+1] = key;
    }
}
