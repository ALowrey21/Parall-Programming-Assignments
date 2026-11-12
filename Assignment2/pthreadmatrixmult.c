#include "assignment2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

//define constant values
#define SIZE1 1024
#define SIZE2 2048
//used for non-divisable dimensions 
#define SIZE3 1000
#define SIZE4 1536

//declartion of functions

void timeSequential(int m, int k, int n, double mA[], double mB[], double mC[]);
void sequenMM(int m, int k, int n, double mA[], double mB[], double mC[], int b);
void matrixCReset(int m, int n, int matrix[]);


int main(){
    //reference: generate_matrix(int rows, int cols, uint64_t seed)
    //arrays of size 1024 
    double *array1A = generate_matrix(SIZE1, SIZE1, 1);
    double *array1B = generate_matrix(SIZE1, SIZE1, 2);
    double *array1C = calloc(SIZE1 * SIZE1 * sizeof(double));


    //arrays of size 2048
    double *array2A = generate_matrix(SIZE2, SIZE2, 1);
    double *array2B = generate_matrix(SIZE2, SIZE2, 2);
    double *array2C = calloc(SIZE2 * SIZE2 * sizeof(double));
    
    
    
    
    //DON'T FORGET TO FREE THE ARRAYS: THEY WANT TO GO INTO THE GREAT BEYOND
    //Do it like this: free(array);
    free(array1A);
    free(array1B);
    free(array1C);
    free(array2A);
    free(array2B);
    free(array2C);
    return 0;
}

//Sequential Implementation with b x b tiles
void sequenMM(int m, int k, int n, double mA[], double mB[], double mC[], int b){
    if(b == 0) {
        int big = (m > k) ? m : k;
        big = (big > n) ? big : n;
        b = big;
    }
    
    //the actual math: arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
    for (int m_floor = 0; m_floor < m; m_floor += b) {
        int m_end = (m_floor + b < m) ? m_floor + b : m;
        
        for(int k_floor = 0; k_floor < k; k_floor +=b) {
            int k_end = (k_floor + b < k) ? k_floor + b: k;
            
            for(int n_floor = 0; n_floor < n; n_floor +=b) {
                int n_end = (n_floor + b < n) ? n_floor + b: n;

                for(int i = m_floor; i < m_end; i++) {
                    
                    for(int l = k_floor; l < k_end; l++) {
                        
                        for(int j = n_floor; j < n_end; j++) {
                            mC[i * n + j] += mA[i * k + l] * mB[l * n + j];    
                        }
                    }
                }
            }
        }

    }
    
}


//Timing sequential
void timeSequential(int m, int k, int n, double mA[], double mB[], double mC[]){
    

}

//CReset Function copied from assignment1
void matrixCReset(int m, int n, int matrix[]) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            arrayMatrix[i * n + j] = 0;
        }
    }
}

