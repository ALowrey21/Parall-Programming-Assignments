#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "lowreytiming.h"
#include "plot.h"

#define SIZE1 256
#define SIZE2 512
#define SIZE3 1024

//decleration of functions
void matrixGeneration(int m, int k, int arrayMatrix[]);
void matrixMultiplicationIJL(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixMultiplicationILJ(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixMultiplicationJIL(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixMultiplicationJLI(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixMultiplicationLIJ(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixMultiplicationLJI(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixCReset(int m, int n, int arrayMatrix[]);
void timeTestIJL(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void timeTestILJ(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void timeTestJIL(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void timeTestJLI(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void timeTestLIJ(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void timeTestLJI(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]);
void matrixComparison(int m, int n, int k, int intArray1[], int intArray2[], int inputA[], int inputB[]);

/*
NOTE: VERY IMPORTANT
in the pdf for the assignment, the three value used in the loops are i, j, and k.
As k is also used for the size of the matrixes, I changed k to l. purely a name 
change, they still work the same.

*/


int main() {
    srand(time(NULL));

    //create each array
    int *intArray1A = malloc(SIZE1 * SIZE1 * sizeof(int));
    int *intArray1B = malloc(SIZE1 * SIZE1 * sizeof(int));
    int *intArray1C = malloc(SIZE1 * SIZE1 * sizeof(int));
    int *intArray2A = malloc(SIZE2 * SIZE2 * sizeof(int));
    int *intArray2B = malloc(SIZE2 * SIZE2 * sizeof(int));
    int *intArray2C = malloc(SIZE2 * SIZE2 * sizeof(int));
    int *intArray3A = malloc(SIZE3 * SIZE3 * sizeof(int));
    int *intArray3B = malloc(SIZE3 * SIZE3 * sizeof(int));
    int *intArray3C = malloc(SIZE3 * SIZE3 * sizeof(int));

    //alternate array c for testing comparisons of orders of i, j, and l. Using SIZE1
    int *alternate1C = malloc(SIZE1 * SIZE1 * sizeof(int));

    //the special arrays!!
    int *specialArrayA = malloc(SIZE1 * SIZE2 * sizeof(int));
    int *specialArrayB = malloc(SIZE2 * SIZE3 * sizeof(int));
    int *specialArrayC = malloc(SIZE1 * SIZE3 * sizeof(int));

    //fill C with zeros at the beginning. Because A and B are never changed after original generation, no reset is necissary 
    matrixCReset(SIZE1, SIZE1, intArray1C);
    matrixCReset(SIZE2, SIZE2, intArray2C);
    matrixCReset(SIZE3, SIZE3, intArray3C);
    matrixCReset(SIZE1, SIZE1, alternate1C);
    matrixCReset(SIZE1, SIZE3, specialArrayC);



    //fill arrays A and B
    matrixGeneration(SIZE1, SIZE1, intArray1A);
    matrixGeneration(SIZE1, SIZE1, intArray1B);
    matrixGeneration(SIZE2, SIZE2, intArray2A); 
    matrixGeneration(SIZE2, SIZE2, intArray2B);
    matrixGeneration(SIZE3, SIZE3, intArray3A);
    matrixGeneration(SIZE3, SIZE3, intArray3B);
    matrixGeneration(SIZE1, SIZE2, specialArrayA);
    matrixGeneration(SIZE2, SIZE3, specialArrayB);

    //Testing Time!!
    //Is the multiplication equal for each iteration?
    printf("\nTesting of the special arrays\n");

    matrixComparison(SIZE1, SIZE1, SIZE1, intArray1C, alternate1C, intArray1A, intArray1B);

    //Testing the special array with JIL
    timeTestJIL(SIZE3, SIZE2, SIZE1, specialArrayA, specialArrayB, specialArrayC);
 
//------------------------------------------------------------------------------------   Start of IJL
    
    //Testing of matrix A[256, 256], B[256, 256], and C[256, 256] with IJL
    printf("\nTesting of matrixes of 256 x 256 with IJL\n");

    timeTestIJL(SIZE1, SIZE1, SIZE1, intArray1A, intArray1B, intArray1C);

//------------------------------------------------------------------------------------   
    
    //Testing of matrix A[512, 512], B[512, 512], C[512, 512] with IJL
    printf("\nTesting of matrixes of 512 x 512 with IJL\n");
    timeTestIJL(SIZE2, SIZE2, SIZE2, intArray2A, intArray2B, intArray2C);    
    
//------------------------------------------------------------------------------------ 

    //Testing of matrix A[1024, 1024], B[1024, 1024], C[1024, 1024] with IJL
    printf("Testing of matrixes of 1024 x 1024 with IJL");
    timeTestIJL(SIZE3, SIZE3, SIZE3, intArray3A, intArray3B, intArray3C);

//------------------------------------------------------------------------------------   Start of ILJ
    
    //Testing of matrix A[256, 256], B[256, 256], and C[256, 256] with ILJ
    printf("\nTesting of matrixes of 256 x 256 with ILJ\n");
    timeTestILJ(SIZE1, SIZE1, SIZE1, intArray1A, intArray1B, intArray1C);

//------------------------------------------------------------------------------------   
    
    //Testing of matrix A[512, 512], B[512, 512], C[512, 512] with ILJ
    printf("\nTesting of matrixes of 512 x 512 with ILJ\n");
    timeTestILJ(SIZE2, SIZE2, SIZE2, intArray2A, intArray2B, intArray2C);    
    
//------------------------------------------------------------------------------------ 

    //Testing of matrix A[1024, 1024], B[1024, 1024], C[1024, 1024] with ILJ
    printf("Testing of matrixes of 1024 x 1024 with ILJ");
    timeTestILJ(SIZE3, SIZE3, SIZE3, intArray3A, intArray3B, intArray3C);
    
//------------------------------------------------------------------------------------   Start of JIL
    
    //Testing of matrix A[256, 256], B[256, 256], and C[256, 256] with JIL
    printf("\nTesting of matrixes of 256 x 256 with JIL\n");
    timeTestJIL(SIZE1, SIZE1, SIZE1, intArray1A, intArray1B, intArray1C);

//------------------------------------------------------------------------------------   
    
    //Testing of matrix A[512, 512], B[512, 512], C[512, 512] with JIL
    printf("\nTesting of matrixes of 512 x 512 with JIL\n");
    timeTestJIL(SIZE2, SIZE2, SIZE2, intArray2A, intArray2B, intArray2C);    
    
//------------------------------------------------------------------------------------ 

    //Testing of matrix A[1024, 1024], B[1024, 1024], C[1024, 1024] with JIL
    printf("Testing of matrixes of 1024 x 1024 with JIL");
    timeTestJIL(SIZE3, SIZE3, SIZE3, intArray3A, intArray3B, intArray3C);

//------------------------------------------------------------------------------------  Start of JLI
    
    //Testing of matrix A[256, 256], B[256, 256], and C[256, 256] with JLI
    printf("\nTesting of matrixes of 256 x 256 with JLI\n");
    timeTestJLI(SIZE1, SIZE1, SIZE1, intArray1A, intArray1B, intArray1C);

//------------------------------------------------------------------------------------   
    
    //Testing of matrix A[512, 512], B[512, 512], C[512, 512] with JLI
    printf("\nTesting of matrixes of 512 x 512 with JLI\n");
    timeTestJLI(SIZE2, SIZE2, SIZE2, intArray2A, intArray2B, intArray2C);    
    
//------------------------------------------------------------------------------------ 

    //Testing of matrix A[1024, 1024], B[1024, 1024], C[1024, 1024] with JLI
    printf("Testing of matrixes of 1024 x 1024 with JLI");
    timeTestJLI(SIZE3, SIZE3, SIZE3, intArray3A, intArray3B, intArray3C);

//------------------------------------------------------------------------------------   Start of LIJ
    
    //Testing of matrix A[256, 256], B[256, 256], and C[256, 256] with LIJ
    printf("\nTesting of matrixes of 256 x 256 with LIJ\n");
    timeTestLIJ(SIZE1, SIZE1, SIZE1, intArray1A, intArray1B, intArray1C);

//------------------------------------------------------------------------------------   
    
    //Testing of matrix A[512, 512], B[512, 512], C[512, 512] with LIJ
    printf("\nTesting of matrixes of 512 x 512 with LIJ\n");
    timeTestLIJ(SIZE2, SIZE2, SIZE2, intArray2A, intArray2B, intArray2C);    
    
//------------------------------------------------------------------------------------ 

    //Testing of matrix A[1024, 1024], B[1024, 1024], C[1024, 1024] with LIJ
    printf("Testing of matrixes of 1024 x 1024 with LIJ");
    timeTestLIJ(SIZE3, SIZE3, SIZE3, intArray3A, intArray3B, intArray3C);

//------------------------------------------------------------------------------------   Start of LJI
    
    //Testing of matrix A[256, 256], B[256, 256], and C[256, 256] with LJI
    printf("\nTesting of matrixes of 256 x 256 with LJI\n");
    timeTestLJI(SIZE1, SIZE1, SIZE1, intArray1A, intArray1B, intArray1C);

//------------------------------------------------------------------------------------   
    
    //Testing of matrix A[512, 512], B[512, 512], C[512, 512] with LJI
    printf("\nTesting of matrixes of 512 x 512 with LJI\n");
    timeTestLJI(SIZE2, SIZE2, SIZE2, intArray2A, intArray2B, intArray2C);    
    
//------------------------------------------------------------------------------------ 

    //Testing of matrix A[1024, 1024], B[1024, 1024], C[1024, 1024] with LJI
    printf("Testing of matrixes of 1024 x 1024 with LJI");
    timeTestLJI(SIZE3, SIZE3, SIZE3, intArray3A, intArray3B, intArray3C);
    
    return 0;
}

void matrixGeneration(int m ,int k, int arrayMatrix[]) {
    //the values used for looping will be i, j, and l. l is used because k is used in the measurements of A and B
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < k; j++) {
            arrayMatrix[i * k + j] = rand() % (int)(sqrt(RAND_MAX));
        }
    }
}

void matrixMultiplicationIJL(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            for(int l = 0; l < k; l++) {
                arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
            }
        }
    }
}

void matrixMultiplicationILJ(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < m; i++) {
        for(int l = 0; l < k; l++) {
            for(int j = 0; j < n; j++) {
                arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
            }
        }
    }
}

void matrixMultiplicationJIL(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int j = 0; j < n; j++) {
        for(int i = 0; i < m; i++) {
            for(int l = 0; l < k; l++) {
                arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
            }
        }
    }
}

void matrixMultiplicationJLI(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int j = 0; j < n; j++) {
        for(int l = 0; l < k; l++) {
            for(int i = 0; i < m; i++) {
                arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
            }
        }
    }
}

void matrixMultiplicationLIJ(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int l = 0; l < k; l++) {
        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
            }
        }
    }
}

void matrixMultiplicationLJI(int m, int k, int n, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int l = 0; l < k; l++) {
        for(int j = 0; j < n; j++) {
            for(int i = 0; i < m; i++) {
                arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
            }
        }
    }
}



void matrixCReset(int m, int n, int arrayMatrix[]) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            arrayMatrix[i * n + j] = 0;
        }
    }
}


void timeTestIJL(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < 5; i++) {
        start();
        matrixMultiplicationIJL(m, n, k, arrayMatrixA, arrayMatrixB, arrayMatrixC);
        stop();
        elapsedTime();


        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        long long flops = ((long long)2 * (long long)m * (long long)n * (long long)k) / t;
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nFLOPS: %lld\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, flops);
        //reset the array 
        matrixCReset(m, n, arrayMatrixC);
    }
}

void timeTestILJ(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < 5; i++) {
        start();
        matrixMultiplicationILJ(m, n, k, arrayMatrixA, arrayMatrixB, arrayMatrixC);
        stop();
        elapsedTime();


        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        long long flops = ((long long)2 * (long long)m * (long long)n * (long long)k) / t;
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nFLOPS: %lld\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, flops);
        //reset the array 
        matrixCReset(m, n, arrayMatrixC);
    }
}

void timeTestJIL(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < 5; i++) {
        start();
        matrixMultiplicationJIL(m, n, k, arrayMatrixA, arrayMatrixB, arrayMatrixC);
        stop();
        elapsedTime();


        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        long long flops = ((long long)2 * (long long)m * (long long)n * (long long)k) / t;
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nFLOPS: %lld\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, flops);
        //reset the array 
        matrixCReset(m, n, arrayMatrixC);
    }
}

void timeTestJLI(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < 5; i++) {
        start();
        matrixMultiplicationJLI(m, n, k, arrayMatrixA, arrayMatrixB, arrayMatrixC);
        stop();
        elapsedTime();


        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        long long flops = ((long long)2 * (long long)m * (long long)n * (long long)k) / t;
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nFLOPS: %lld\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, flops);
        //reset the array 
        matrixCReset(m, n, arrayMatrixC);
    }
}

void timeTestLIJ(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < 5; i++) {
        start();
        matrixMultiplicationLIJ(m, n, k, arrayMatrixA, arrayMatrixB, arrayMatrixC);
        stop();
        elapsedTime();


        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        long long flops = ((long long)2 * (long long)m * (long long)n * (long long)k) / t;
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nFLOPS: %lld\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, flops);
        //reset the array 
        matrixCReset(m, n, arrayMatrixC);
    }
}

void timeTestLJI(int m, int n, int k, int arrayMatrixA[], int arrayMatrixB[], int arrayMatrixC[]) {
    for(int i = 0; i < 5; i++) {
        start();
        matrixMultiplicationLJI(m, n, k, arrayMatrixA, arrayMatrixB, arrayMatrixC);
        stop();
        elapsedTime();


        double t = elapsed.tv_sec + (elapsed.tv_nsec * 1e-9);
        long long flops = ((long long)2 * (long long)m * (long long)n * (long long)k) / t;
        printf("\nTest %d \nElapsed Seconds: %ld \nElapsed NanoSeconds: %ld \nFLOPS: %lld\n", i + 1, elapsed.tv_sec, elapsed.tv_nsec, flops);
        //reset the array 
        matrixCReset(m, n, arrayMatrixC);
    }
}

void matrixComparison(int m, int n, int k, int intArray1[], int intArray2[], int inputA[], int inputB[]) {
    int equal = 1;
    int value1;
    int value2;
    matrixMultiplicationIJL(m, n, k, inputA, inputB, intArray1);
    matrixMultiplicationJLI(m, n, k, inputA, inputB, intArray2);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            value1 =intArray1[i * n + j];
            value2 = intArray2[i * n +j];
            if(value1 != value2){
                equal = 0;
            }
        }
    }


    if(equal == 1) {
        printf("\nThe arrays made by two diferent orders of i, j, and l are still equal\n");
    } else {
        printf("\nThe arrays made by two different orders of i, j, and l are NOT equa;\n");
    }
}




