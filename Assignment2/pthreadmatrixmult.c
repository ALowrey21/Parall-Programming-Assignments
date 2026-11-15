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

#define REPEATCOUNT 11

typedef struct{ 
    int start;
    int end;
    int k;
    int n;
    double *mA;
    double *mB;
    double *mC;
    int b;
} oneDArgs;

typedef struct{
    int rowStart;
    int rowEnd;
    int colStart;
    int colEnd;
    int n;
    int k;
    double *mA;
    double *mB;
    double *mC;
    int b;
} twoDArgs;

typedef struct{
    double timed;
    int b;
} timeB;

//declartion of functions


void sequenMM(int m, int k, int n, double mA[], double mB[], double mC[], int b);
void oneDMM(int start, int end, int k, int n, double mA[], double mB[], double mC[], int b);
void twoDMM(int rowStart, int rowEnd, int colStart, int colEnd, int n, int k, double mA[], double mB[], double mC[], int b);

double timeSequential(int m, int k, int n, double mA[], double mB[], double mC[], int b);
double time1D(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p);
double time2D(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p, int pr);

double findMedian(double times[]);
void matrixCReset(int m, int n, double matrix[]);
void *oneDWrapper(void *arg);
void *twoDWrapper(void *arg);


int main(){
    //reference: generate_matrix(int rows, int cols, uint64_t seed)
    //arrays of size 1024 
    double *array1A = generate_matrix(SIZE1, SIZE1, 1);
    double *array1B = generate_matrix(SIZE1, SIZE1, 2);
    double *array1C = calloc(SIZE1 * SIZE1, sizeof(double));



    //arrays of size 2048
    double *array2A = generate_matrix(SIZE2, SIZE2, 1);
    double *array2B = generate_matrix(SIZE2, SIZE2, 2);
    double *array2C = calloc(SIZE2 * SIZE2, sizeof(double));

    //sequential median time;
    double seqMed1024 = timeSequential(SIZE1, SIZE1, SIZE1, array1A, array1B, array1C, 0);
    printf("\n-----------\nTseq of M=K=N=1024 with b = 0 on a sequential function is: %f\n", seqMed1024);

    double seqMed2048 = timeSequential(SIZE2, SIZE2, SIZE2, array2A, array2B, array2C, 0);
    printf("\n-----------\nTseq of M=N=K=2048 with b = 0 on a sequential function is: %f\n", seqMed2048);

    
    
    
    
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
    //if b is 0, so no tiles, set the number of tiles to one
    int m_full_blocks;
    int k_full_blocks;
    int n_full_blocks;
    if(b == 0) {
        m_full_blocks = 1;
        k_full_blocks = 1;
        n_full_blocks = 1;
    } else {
        m_full_blocks = m / b;
        k_full_blocks = k / b;
        n_full_blocks = n / b;
    }
    
    
    //the actual math: arrayMatrixC[i * n + j] += arrayMatrixA[i * k + l] * arrayMatrixB[l * n + j];
    //new loop
    for (int m_block = 0; m_block < m_full_blocks; m_block++) {
        int m_floor = m_block * b;
        int m_end = (m_block + 1 == m_full_blocks) ? m : m_floor + b;
        
        for (int k_block = 0; k_block < k_full_blocks; k_block++) {
            int k_floor = k_block * b;
            int k_end = (k_block + 1 == k_full_blocks) ? k : k_floor + b;

            for (int n_block = 0; n_block < n_full_blocks; n_block++) {
                int n_floor = n_block * b;
                int n_end = (n_block + 1 == n_full_blocks) ? n : n_floor + b;

                for (int i = m_floor; i < m_end; i++) {
                    
                    for (int l = k_floor; l < k_end; l++) {
                        
                        for (int j = n_floor; j < n_end; j++) {
                            mC[i * n + j] += mA[i * k + l] * mB[l * n + j];    
                        }
                    }
                }
            }
        }
    }
    
}

void oneDMM(int start, int end, int k, int n, double mA[], double mB[], double mC[], int b) {
    //if b is 0, so no tiles, set the number of tiles to one
    int m_full_blocks;
    int k_full_blocks;
    int n_full_blocks;
    if(b == 0) {
        m_full_blocks = 1;
        k_full_blocks = 1;
        n_full_blocks = 1;
    } else {
        m_full_blocks = (end - start) / b;
        k_full_blocks = k / b;
        n_full_blocks = n / b;
    }
    m_full_blocks = (m_full_blocks == 0) ? 1 : m_full_blocks;
    for(int m_block = 0; m_block < m_full_blocks; m_block++) {
        int m_floor = start + (m_block * b);
        int m_end = (m_block + 1 == m_full_blocks) ? end : m_floor + b;

        for (int k_block = 0; k_block < k_full_blocks; k_block++) {
            int k_floor = k_block * b;
            int k_end = (k_block + 1 == k_full_blocks) ? k : k_floor + b;

            for (int n_block = 0; n_block < n_full_blocks; n_block++) {
                int n_floor = n_block * b;
                int n_end = (n_block + 1 == n_full_blocks) ? n : n_floor + b;

                for (int i = m_floor; i < m_end; i++) {
                    
                    for (int l = k_floor; l < k_end; l++) {
                        
                        for (int j = n_floor; j < n_end; j++) {
                            mC[i * n + j] += mA[i * k + l] * mB[l * n + j];    
                        }
                    }
                }
            }
        }
    }
}

void twoDMM(int rowStart, int rowEnd, int colStart, int colEnd, int n, int k, double mA[], double mB[], double mC[], int b) {
    int m_full_blocks;
    int k_full_blocks;
    int n_full_blocks;
    if(b == 0) {
        m_full_blocks = 1;
        k_full_blocks = 1;
        n_full_blocks = 1;
    } else {
        m_full_blocks = (rowEnd - rowStart) / b;
        k_full_blocks = k / b;
        n_full_blocks = (colEnd - colStart) / b;
    }
    m_full_blocks = (m_full_blocks == 0) ? 1 : m_full_blocks;
    n_full_blocks = (n_full_blocks == 0) ? 1 : n_full_blocks;

    for(int m_block = 0; m_block < m_full_blocks; m_block++) {
        int m_floor = rowStart + (m_block * b);
        int m_end = (m_block + 1 == m_full_blocks) ? rowEnd : m_floor + b;

        for (int k_block = 0; k_block < k_full_blocks; k_block++) {
            int k_floor = k_block * b;
            int k_end = (k_block + 1 == k_full_blocks) ? k : k_floor + b;

            for (int n_block = 0; n_block < n_full_blocks; n_block++) {
                int n_floor = colStart + (n_block * b);
                int n_end = (n_block + 1 == n_full_blocks) ? colEnd : n_floor + b;

                for (int i = m_floor; i < m_end; i++) {
                    
                    for (int l = k_floor; l < k_end; l++) {
                        
                        for (int j = n_floor; j < n_end; j++) {
                            mC[i * n + j] += mA[i * k + l] * mB[l * n + j];    
                        }
                    }
                }
            }
        }
    }
}

//Timing sequential
double timeSequential(int m, int k, int n, double mA[], double mB[], double mC[], int b){

    double times[REPEATCOUNT];

    
    for (int i = 0; i < REPEATCOUNT; i++) {
        matrixCReset(m, n, mC);
        double untiledTimeStart = now_seconds();
        sequenMM(m, k, n, mA, mB, mC, b);
        double untiledTimeEnd = now_seconds();
        double untiledTime = untiledTimeEnd - untiledTimeStart;
        double rate = 2 * m * k * n / untiledTime;
        printf("\nAttempt: %d of b: %d\n", i, b);
        printf("Elapsed Time for Sequential: %f\n", untiledTime);
        printf("Rate in FLOPS: %d", rate);
        
        
        times[i] = untiledTime;
        
        
    }
    //return the median of the times
    return findMedian(times);

}

//Timing One Dimensional
double time1D(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p) {
    double times[REPEATCOUNT];
    pthread_t threads[p];
    oneDArgs sections[p];
    int size = m / p;
    for(int i = 0; i < p; i++) {   
        sections[i].start = size * i;
        if(i == p - 1) {
            sections[i].end = m;
        } else {
            sections[i].end = sections[i].start + size;
        }
        sections[i].k = k;
        sections[i].n = n;
        sections[i].mA = mA;
        sections[i].mB = mB;
        sections[i].mC = mC;
        sections[i].b = b;
    }

    for(int i = 0; i < REPEATCOUNT; i++) {
        matrixCReset(m, n, mC);
        double timeStart = now_seconds();
        for (int j = 0; j < p; j++) {
            pthread_create(&threads[j], NULL, oneDWrapper, &sections[j]);
        }
        for (int j = 0; j < p; j++) {
            pthread_join(threads[j], NULL);
        }
        double timeEnd = now_seconds();
        double timing = timeEnd - timeStart;
        printf("\nAttempt: %d with b: %d and p: %d\n", i, b, p);
        printf("Elapsed Time for One-Dimension: %f\n", timing);
        times[i] = timing;
    }
    return findMedian(times);
}

//Timing Two Dimensional
double time2D(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p, int pr) {
    double times[REPEATCOUNT];
    pthread_t threads[p];
    twoDArgs sections[p];

    int pc = p / pr;
    //Row Band Height
    int rbh = m / pr;
    //Column Band Width
    int cbw = n / pc;

    //creation of the sections that each thread will cover
   
    for (int i = 0; i < pr; i++) {
        for(int j = 0; j < pc; j++) {
            int s = i * pc + j;
            sections[s].rowStart = rbh * i;
            sections[s].colStart = cbw * j;
            if(i == pr - 1) {
                sections[s].rowEnd = m;
            } else {
                sections[s].rowEnd = sections[s].rowStart + rbh;
            }
            if(j == pc - 1) {
                sections[s].colEnd = n;
            } else {
                sections[s].colEnd = sections[s].colStart + cbw;
            }
        sections[s].k = k;
        sections[s].n = n;
        sections[s].mA = mA;
        sections[s].mB = mB;
        sections[s].mC = mC;
        sections[s].b = b;

        }
    }

    for(int i = 0; i < REPEATCOUNT; i++) {
        matrixCReset(m, n, mC);
        double timeStart = now_seconds();
        for(int j = 0; j < p; j++) {
            pthread_create(&threads[j], NULL, twoDWrapper, &sections[j]);
        }
        for(int j = 0; j < p; j++) {
            pthread_join(threads[j], NULL);
        }
        double timeEnd = now_seconds();
        double timing = timeEnd - timeStart;
        printf("\nAttempt: %d with b: %d and p: %d\n", i, b, p);
        printf("Dimensions of p are %d x %d\n", pr, pc);
        printf("Elapsed Time for Two-Dimension: %f\n", timing);
        times[i] = timing;
    }

    return findMedian(times);
}


//Testing the Seqential and Finding b*
int testSeqen(int m, int k, int n, double mA[], double mB[], double mC[], double tSeq) {
    int bees[6] = {0, 16, 32, 128, 64, 256};
    timeB bTimes[6];

    for(int i = 0; i < 6, i++) {
        bTimes[i].timed = timeSequential(m, k, n, mA, mB, mC, bees[i]);
        bTimes[i].b = bees[i];
    }
    double smallTime = INFINITY;
    int smallB = -1;
    for(int i = 0; i < 6; i++) {
        if(bTimes[i].timed < smallTime) {
            smallTime = bTimes[i].timed;
            smallB = bTimes[i].b;
        }
    }
    printf("\nThe b with the smallest median time is %d with a time of %f\n", smallB, smallTime);
    return smallB;
}



double findMedian(double times[]) {
    //sort for median time

    for(int i = 0; i < REPEATCOUNT; i++) {
        double current = times[i];
        int j = i - 1;

        while(j >= 0 && times[j] > current) {
            times[j+1] = times[j];
            j--;
        }
        times[j + 1] = current;
    }

    //median time of the runs
    double median = times[REPEATCOUNT / 2];
    return median;
}



//CReset Function copied from assignment1
void matrixCReset(int m, int n, double matrix[]) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            matrix[i * n + j] = 0;
        }
    }
}

//for pthreads calling
void *oneDWrapper(void *arg) {
    oneDArgs *args = (oneDArgs *) arg;
    oneDMM(args->start, args->end, args->k, args->n, args->mA, args->mB, args->mC, args->b);
    return NULL;
}

//for pthread calling
void *twoDWrapper(void *arg) {
    twoDArgs *args = (twoDArgs *) arg;
    twoDMM(args->rowStart, args->rowEnd, args->colStart, args->colEnd, args->n, args->k, args->mA, args->mB, args->mC, args->b);
    return NULL;
}

/* 
In case I need it for later:
original loop
    for (int m_floor = 0; m_floor < m; m_floor += b) {
        //pretty much if it is the last section, do only the remaining, don't try past the limits of the matrix
        int m_end = (m_floor + b < m) ? m_floor + b : m;
        
        for (int k_floor = 0; k_floor < k; k_floor +=b) {
            int k_end = (k_floor + b < k) ? k_floor + b: k;
            
            for (int n_floor = 0; n_floor < n; n_floor +=b) {
                int n_end = (n_floor + b < n) ? n_floor + b: n;

                for (int i = m_floor; i < m_end; i++) {
                    
                    for (int l = k_floor; l < k_end; l++) {
                        
                        for (int j = n_floor; j < n_end; j++) {
                            mC[i * n + j] += mA[i * k + l] * mB[l * n + j];    
                        }
                    }
                }
            }
        }

    }
*/