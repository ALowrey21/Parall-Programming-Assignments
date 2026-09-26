#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "lowreytiming.h"
#include "plot.h"

#define SIZE6 1000000
#define SIZE7 10000000
#define SIZE8 100000000

//declaration of functions
void intArraySum(int intArray[], int size);
void doubleArraySum(double doubleArray[], int size);
void testTimeint(int intArray[], int size, int intArraycpy[], size_t bytesize);
void testTimedouble(double doubleArray[], int size, double doubleArraycpy[], size_t bytesize);
void randomNumbersInt(int intArray[], int size);
void randomNumbersDouble(double doubleArray[], int size);



int main() {
    //this code until I tell is copied over from ArrayMaximum
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
    //Copied code ends here

    //Testing

    //Test of random int array with size of 10 ^ 6
    printf("\nTesting of intArray6\n");
    testTimeint(intArray6, SIZE6, intArray6cpy, intn6);
//----------------------------------------------------------------------
    
    //Test of random int array with size of 10^7
    printf("\nTesting of intArray7\n");
    testTimeint(intArray7, SIZE7, intArray7cpy, intn7);

//----------------------------------------------------------------------
    
    //Test of random int array with size of 10^8
    printf("\nTesting of intArray8\n");
    testTimeint(intArray8, SIZE8, intArray8cpy, intn8);

 //----------------------------------------------------------------------

    //Test of random double array with size of 10^6
    printf("\nTesting of doubleArray6\n");
    testTimedouble(doubleArray6, SIZE6, doubleArray7cpy, doubln6);

 //----------------------------------------------------------------------

    //Test of random double array with size of 10^7
    printf("\nTesting of doubleArray7\n");
    testTimedouble(doubleArray7, SIZE7, doubleArray7cpy, doubln7);

//----------------------------------------------------------------------

    //Test of random double array with size of 10^8
    printf("\nTesting of doubleArray8\n");
    testTimedouble(doubleArray8, SIZE8, doubleArray8cpy, doubln8);
    
    
    
    return 0;
}

void intArraySum(int intArray[], int size) {
    int sum = 0;
    for(int i = 0; i < size; i++) {
        int temp = intArray[i];
        intArray[i] = sum;
        sum += temp;
    }
}

void doubleArraySum(double doubleArray[], int size) {
    double sum = 0;
    for(int i = 0; i < size; i++) {
        double temp = doubleArray[i];
        doubleArray[i] = sum;
        sum += temp;
    }
}

void testTimeint(int intArray[], int size, int intArraycpy[], size_t bytesize) {
    for(int i = 0; i < 5; i++) {
        start();
        intArraySum(intArray, size);
        stop();
        elapsedTime();
        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        int element_rate = (int) (size / t);
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nElement Rate: %d\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, element_rate);
        //reset the array 
        memcpy(intArray, intArraycpy, bytesize);
    }
}

void testTimedouble(double doubleArray[], int size, double doubleArraycpy[], size_t bytesize) {
    for(int i = 0; i < 5; i++) {
        start();
        doubleArraySum(doubleArray, size);
        stop();
        elapsedTime();
        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        int element_rate = (int) (size / t);
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nElement Rate: %d\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, element_rate);
        //reset the array 
        memcpy(doubleArray, doubleArraycpy, bytesize);
    }
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