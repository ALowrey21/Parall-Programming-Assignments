#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "timing.h"

//sizes for the arrays for each size. Number is the power that 10 is to
#define SIZE6 1000000
#define SIZE7 10000000
#define SIZE8 100000000

//declaration of functions
int maxIntegerA(int intArray[], int size);
int maxIntegerB(int intArray[], int size);
double maxDoubleA(double doubleArray[], int size);
double maxDoubleB(double doubleArray[], int size);
void randomNumbersInt(int intArray[], int size);
void randomNumbersDouble(double doubleArray[], int size);



int main() {
    //initialization of all of the arrays, both int and double

    srand(time(NULL));
    int *intArray6 = malloc(SIZE6 * sizeof(int));
    int *intArray7 = malloc(SIZE7 * sizeof(int));
    int *intArray8 = malloc(SIZE8 * sizeof(int));
    double *doubleArray6 = malloc(SIZE6 * sizeof(double));
    double *doubleArray7 = malloc(SIZE7 * sizeof(double));
    double *doubleArray8 = malloc(SIZE8 * sizeof(double));
    

    //finding size in bytes of each array for later
    size_t intn6 = sizeof(intArray6[0]) * SIZE6;
    size_t intn7 = sizeof(intArray7[0]) * SIZE7;
    size_t intn8 = sizeof(intArray8[0]) * SIZE8;
    size_t doubln6 = sizeof(doubleArray6[0]) * SIZE6;
    size_t doubln7 = sizeof(doubleArray7[0]) * SIZE7;
    size_t doubln8 = sizeof(doubleArray8[0]) * SIZE8;

    //put random numbers in each of the arrays
    randomNumbersInt(intArray6, SIZE6);
    randomNumbersInt(intArray7, SIZE7);
    randomNumbersInt(intArray8, SIZE8);
    randomNumbersDouble(doubleArray6, SIZE6);
    randomNumbersDouble(doubleArray7, SIZE7);
    randomNumbersDouble(doubleArray8, SIZE8);

    //create copies of each array
    int *intArray6cpy = malloc(SIZE6 * sizeof(int));
    int *intArray7cpy = malloc(SIZE7 * sizeof(int));
    int *intArray8cpy = malloc(SIZE8 * sizeof(int));    
    double *doubleArray6cpy = malloc(SIZE6 * sizeof(double));
    double *doubleArray7cpy = malloc(SIZE7 * sizeof(double));
    double *doubleArray8cpy = malloc(SIZE8 * sizeof(double));

    //copy the starting info of each array into its respective copy
 
    memcpy(intArray6cpy, intArray6, intn6);
    memcpy(intArray7cpy, intArray7, intn7);
    memcpy(intArray8cpy, intArray8, intn8);
    memcpy(doubleArray6cpy, doubleArray6, doubln6);
    memcpy(doubleArray7cpy, doubleArray7, doubln7);
    memcpy(doubleArray8cpy, doubleArray8, doubln8);
    
    struct timespec {
        time_t seconds;
        long nanoseconds;
    };
    //NOW ONTO TESTING

    //start the clock
    start();
    maxIntegerA(intArray6, SIZE6);
    stop()
    double time1 = elapsedMilliseconds();
    printf("%f \n", time1);
    //TODO: create my own timing file for c
    



    
    return 0;
}

int maxIntegerA(int intArray[], int size) {
    //Integer Array using version A
    int max = intArray[0];

    for(int i = 0; i < size; i++) {
        if(intArray[i] > max) {
            max = intArray[i];
        }
    }
    return max;
}

int maxIntegerB(int intArray[], int size) {
    //Integer Array using version B
    int max = intArray[0];

    for(int i = 0; i < size; i++) {
        max = (intArray[i] > max) ? intArray[i] : max;
    }
    return max;
}

double maxDoubleA(double doubleArray[], int size) {
    //Double Array using A
    double max = doubleArray[0];
    for(int i = 0; i < size; i++) {
        if(doubleArray[i] > max) {
            max = doubleArray[i];
        }
    }
    return max;

}

double maxDoubleB(double doubleArray[], int size) {
    //Double Array using B
    double max = doubleArray[0];

    for(int i = 0; i < size; i++) {
        max = (doubleArray[i] > max) ? doubleArray[i] : max;
    }
    return max;

}

void randomNumbersInt(int intArray[], int size) {
   //puts the random numbers in the array. 
    for(int i = 0; i < size; i++) {
        intArray[i] = rand() % RAND_MAX;
    }
}

void randomNumbersDouble(double doubleArray[], int size) {
   //puts the random numbers in the array. 
    for(int i = 0; i < size; i++) {
        doubleArray[i] = ((double) rand()) / RAND_MAX;
    }
}





