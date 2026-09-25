#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "lowreytiming.h"

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
void testTimeintA(int intArray[], int size, int intArraycpy[], size_t bitsize);
void testTimeintB(int intArray[], int size, int intArraycpy[], size_t bitsize);
void testTimedblA(double doubleArray[], int size, double doubleArraycpy[], size_t bitsize);
void testTimedblB(double doubleArray[], int size, double doubleArraycpy[], size_t bitsize);
void arrayDoubling();



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
    

    //NOW ONTO TESTING


    //testing for random int array with 10 ^ 6 values
    printf("Testing of intArray6 with A\n");
    testTimeintA(intArray6, SIZE6, intArray6cpy, intn6);
    
    printf("\nTesting of intArray6 with B\n");
    testTimeintB(intArray6, SIZE6, intArray6cpy, intn6);

//------------------------------------------------------------
    //testing for random int array with 10 ^ 7 values
    printf("\nTesting of intArray7 with A\n");
    testTimeintA(intArray7, SIZE7, intArray7cpy, intn7);
    
    printf("\nTesting of intArray7 with B\n");
    testTimeintB(intArray7, SIZE7, intArray7cpy, intn7);

 //------------------------------------------------------------   
    //testing for random int array with 10 ^ 8 values
    printf("\nTesting of intArray8 with A\n");
    testTimeintA(intArray8, SIZE8, intArray8cpy, intn8);
    
    printf("\nTesting of intArray8 with B\n");
    testTimeintB(intArray8, SIZE8, intArray8cpy, intn7);

//------------------------------------------------------------
    //testing for double array with 10 ^ 6 values
    printf("\nTesing of doubleArray6 with A\n");
    testTimedblA(doubleArray6, SIZE6, doubleArray6cpy, doubln6);

    printf("\nTesting of doubleArray6 with B\n");
    testTimedblB(doubleArray6, SIZE7, doubleArray6cpy, doubln6);

//------------------------------------------------------------
    //testing for double array with 10 ^ 7 values
    printf("\nTesing of doubleArray7 with A\n");
    testTimedblA(doubleArray7, SIZE7, doubleArray7cpy, doubln7);

    printf("\nTesting of doubleArray7 with B\n");
    testTimedblB(doubleArray7, SIZE7, doubleArray7cpy, doubln7);

//------------------------------------------------------------
    //testing for double array with 10 ^ 8 values
    printf("\nTesing of doubleArray8 with A\n");
    testTimedblA(doubleArray8, SIZE8, doubleArray8cpy, doubln8);

    printf("\nTesting of doubleArray8 with B\n");
    testTimedblB(doubleArray8, SIZE8, doubleArray8cpy, doubln8);

//------------------------------------------------------------
    //testing of the doubling double arrays
    arrayDoubling();



    
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

void testTimeintA(int intArray[], int size, int intArraycpy[], size_t bitsize) {
    for(int i = 0; i < 5; i++) {
        start();
        maxIntegerA(intArray, size);
        stop();
        elapsedTime();
        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        int bandwidth = (int) ((size * sizeof(int)) / t);
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nBandwidth: %d\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, bandwidth);
        //reset the array 
        memcpy(intArray, intArraycpy, bitsize);

    }
}

void testTimeintB(int intArray[], int size, int intArraycpy[], size_t bitsize) {
    for(int i = 0; i < 5; i++) {
        start();
        maxIntegerB(intArray, size);
        stop();
        elapsedTime();
        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        int bandwidth = (int) ((size * sizeof(int)) / t);
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nBandwidth: %d\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, bandwidth);
        //reset the array
        memcpy(intArray, intArraycpy, bitsize);
    }
}

void testTimedblA(double doubleArray[], int size, double doubleArraycpy[], size_t bitsize) {
    for(int i = 0; i < 5; i++) {
        start();
        maxDoubleA(doubleArray, size);
        stop();
        elapsedTime();
        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        int bandwidth = (int) ((size * sizeof(double)) / t);
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nBandwidth: %d\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, bandwidth);
        memcpy(doubleArray, doubleArraycpy, bitsize);
    }
}

void testTimedblB(double doubleArray[], int size, double doubleArraycpy[], size_t bitsize) {
    for(int i = 0; i < 5; i++) {
        start();
        maxDoubleB(doubleArray, size);
        stop();
        elapsedTime();
        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        int bandwidth = (int) ((size * sizeof(double)) / t);
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nBandwidth: %d\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, bandwidth);
        memcpy(doubleArray, doubleArraycpy, bitsize);
    }
}

void arrayDoubling() {
    for(int i = 0; i < 18; i++) {
        int bytesize = 2048 * pow(2, i);
        double *doublingArray = malloc(bytesize);
        double *doublingArraycpy = malloc(bytesize);
        int arrayLength = bytesize / sizeof(double);
        //array is filled with random values
        randomNumbersDouble(doublingArray, arrayLength);
        //create a copy of the array
        memcpy(doublingArraycpy, doublingArray, bytesize);

        printf("\nTesting of double array with byte size of %d and an array length of %d \n", bytesize, arrayLength);
        testTimedblB(doublingArray, arrayLength, doublingArraycpy, bytesize);
        free(doublingArray);
        free(doublingArraycpy);
    }
}





