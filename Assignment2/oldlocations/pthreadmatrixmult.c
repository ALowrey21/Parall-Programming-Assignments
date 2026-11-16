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

//number of times that each test is run
#define REPEATCOUNT 7

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

timeB testSeqen(int m, int k, int n, double mA[], double mB[], double mC[]);
void test1D(int m, int k, int n, double mA[], double mB[], double mC[], double tSeq, int bStar);
void test2D(int m, int k, int n, double mA[], double mB[], double mC[], double tSeq, int bStar);
void testNonDivisible(int m, int k, int n, double mA[], double mB[], double mC[], int bStar);


double findMedian(double times[]);
void matrixCReset(int m, int n, double matrix[]);
void *oneDWrapper(void *arg);
void *twoDWrapper(void *arg);
double *cCompareAgainst(int m, int k, int n, double mA[], double mB[]);
void compareCees(int m, int n, double mC[], double testAgainst[]);


int main(){
    //creation of each of the CSV files
    
    
    
    //reference: generate_matrix(int rows, int cols, uint64_t seed)
    //arrays of size 1024 
    double *array1A = generate_matrix(SIZE1, SIZE1, 1);
    double *array1B = generate_matrix(SIZE1, SIZE1, 2);
    double *array1C = calloc(SIZE1 * SIZE1, sizeof(double));
    double *compare1C = calloc(SIZE1 * SIZE1, sizeof(double));



    //arrays of size 2048
    double *array2A = generate_matrix(SIZE2, SIZE2, 1);
    double *array2B = generate_matrix(SIZE2, SIZE2, 2);
    double *array2C = calloc(SIZE2 * SIZE2, sizeof(double));
    double *compare2C = calloc(SIZE2 * SIZE2, sizeof(double));

    //special arrays
    double *specialA = generate_matrix(SIZE3, SIZE1, 1);
    double *specialB = generate_matrix(SIZE1, SIZE4, 2);
    double *specialC = calloc(SIZE3 * SIZE4, sizeof(double));
    double *CompareSC = calloc(SIZE3 * SIZE4, sizeof(double));

    //sequential median time;
    printf("\nTesting Seqential of M=K=N=1024");
    double seqMed1024 = timeSequential(SIZE1, SIZE1, SIZE1, array1A, array1B, array1C, 0);
    printf("\nTseq of M=K=N=1024 with b = 0 on a sequential function is: %f\n", seqMed1024);
    printf("-------------------------------------------\n");


  

    //testing sequential of M=K=N=2048
    //returns b*
    printf("\nTesting Seqential of M=K=N=2048");
    timeB medAndBStar = testSeqen(SIZE2, SIZE2, SIZE2, array2A, array2B, array2C);
    //the b and timed values in this iteration of timeB do not match. timed is the median of b = 0,
    //and b is the b value with the lowest median time, not necessarily b = 0.
    //They were paired and returned together for ease of use
    
    double seqMed2048 = medAndBStar.timed;
    double bStar = medAndBStar.b;
    printf("\nTseq of M=N=K=2048 with b = 0 on a sequential function is: %f\n", seqMed2048);
    printf("-------------------------------------------\n");


    //testing one-demensional of M=K=N=1024
    printf("\nTesting One Dimensional of M=K=N=1024");
    test1D(SIZE1, SIZE1, SIZE1, array1A, array1B, array1C, seqMed1024, bStar);
    printf("-------------------------------------------\n");

    //testing one-dimennsional of M=K=N=2048
    printf("\nTesting One Dimensional of M=K=N=2048");
    test1D(SIZE2, SIZE2, SIZE2, array2A, array2B, array2C, seqMed2048, bStar);
    printf("-------------------------------------------\n");


    //testing tw0-dimensional of M=K=N=1024
    printf("\nTesting Two Dimentional of M=K=N=1024");
    test2D(SIZE1, SIZE1, SIZE1, array1A, array1B, array1C, seqMed1024, bStar);
    printf("-------------------------------------------\n");


    //testing two-dimensional of M=K=N=2048
    printf("\nTesting Two Dimentional of M=K=N=2048");
    test2D(SIZE2, SIZE2, SIZE2, array2A, array2B, array2C, seqMed2048, bStar);
    printf("-------------------------------------------\n");


    //testing M = 1000, K = 1024, N = 1536
    printf("\nTesting Two Dimentional of M= 1000, K = 1024, N = 1536");
   



    
    //DON'T FORGET TO FREE THE ARRAYS: THEY WANT TO GO INTO THE GREAT BEYOND
    //Do it like this: free(array);
    free(array1A);
    free(array1B);
    free(array1C);
    free(compare1C);
    free(array2A);
    free(array2B);
    free(array2C);
    free(compare2C);
    free(specialA);
    free(specialB);
    free(specialC);
    free(CompareSC);
    
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
        double rate = (double)2 * m * k * n / untiledTime;
        printf("\nAttempt: %d of b: %d\n", i + 1, b);
        printf("Elapsed Time for Sequential: %f\n", untiledTime);
        printf("Rate in FLOPS: %f\n", rate);
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
        printf("\nAttempt: %d with b: %d and p: %d\n", i + 1, b, p);
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
        printf("\nAttempt: %d with b: %d and p: %d\n", i + 1, b, p);
        printf("Dimensions of p are %d x %d\n", pr, pc);
        printf("Elapsed Time for Two-Dimension: %f\n", timing);
        times[i] = timing;
    }

    return findMedian(times);
}


//Testing the Seqential and Finding b*
timeB testSeqen(int m, int k, int n, double mA[], double mB[], double mC[]) {
    int bees[6] = {0, 16, 32, 64, 128, 256};
    timeB bTimes[6];
    //the b and timed values in this iteration of timeB do not match. timed is the median of b = 0,
    //and b is the b value with the lowest median time, not necessarily b = 0.
    //They are paired and returned together for ease of use
    timeB medAndBStar;
    double *check = calloc(m * n, sizeof(double));
    check = cCompareAgainst(m, k, n, mA, mB);

    for(int i = 0; i < 6; i++) {
        bTimes[i].timed = timeSequential(m, k, n, mA, mB, mC, bees[i]);
        bTimes[i].b = bees[i];
        if(i == 0){
            medAndBStar.timed = bTimes[i].timed;
        }
    }
    compareCees(m, n, mC, check);
    double smallTime = INFINITY;
    int smallB = -1;
    for(int i = 0; i < 6; i++) {
        if(bTimes[i].timed < smallTime) {
            smallTime = bTimes[i].timed;
            smallB = bTimes[i].b;
        }
    }
    printf("\nThe b with the smallest median time is %d with a time of %f\n", smallB, smallTime);
    free(check);
    medAndBStar.b = smallB;
    return medAndBStar;
}

//Testing the One-Dimentioal partition
void test1D(int m, int k, int n, double mA[], double mB[], double mC[], double tSeq, int bStar) {
    int pees[7] = {1, 2, 4, 8, 12, 18, 36};
    double *check = calloc(m * n, sizeof(double));
    check = cCompareAgainst(m, k, n, mA, mB);
    
    //run them with b = 0 
    //running once per p in pees array
    for (int j = 0; j < 7; j++) {
        double median = time1D(m, k, n, mA, mB, mC, 0, pees[j]);
        printf("\nWith b =  0 and p = %d:", pees[j]);
        double sp = tSeq / median;
        double rate = (double)2 * m * k * n / median;
        printf("\nT(%d) = %f \nS(%d) = %f rate = %f\n", pees[j], median, pees[j], sp, rate);
    }
    compareCees(m, n, mC, check);

    //run with b = b* 
    //running once per p in pees array
    for (int j = 0; j < 7; j++) {
        double median = time1D(m, k, n, mA, mB, mC, bStar, pees[j]);
        printf("\nWith b =  %d and p = %d:", bStar, pees[j]);
        double sp = tSeq / median;
        double rate = (double)2 * m * k * n / median;
        printf("\nT(%d) = %f \nS(%d) = %f rate = %f\n", pees[j], median, pees[j], sp, rate);
    }


    compareCees(m, n, mC, check);
    free(check);

}

//Testing the Two-Dimensional partition
void test2D(int m, int k, int n, double mA[], double mB[], double mC[], double tSeq, int bStar) {
    //running with bStar
    //inclusion of a second 36 and a 1 is for the 1 x 36 column grid run
    int pees[7] = {2, 4, 8, 12, 18, 36, 36};
    int peers[7] = {1, 2, 2, 3, 3, 6, 1};
    double *check = calloc(m * n, sizeof(double));
    check = cCompareAgainst(m, k, n, mA, mB);

    for (int i = 0; i < 7; i++) {
        double median = time2D(m, k, n, mA, mB, mC, bStar, pees[i], peers[i]);
        double sp = tSeq / median;
        double rate = (double)2 * m * k * n / median;
        printf("\nWith b =  %d and p = %d:", bStar, pees[i]);
        printf("\nT(%d) = %f \nS(%d) = %f rate = %f\n", pees[i], median, pees[i], sp, rate);
    }


    compareCees(m, n, mC, check);
    free(check);
    
   
}

//Testing for Two-Dimentional with non-divisible dimensions
void testNonDivisible(int m, int k, int n, double mA[], double mB[], double mC[], int bStar) {
    double *check = calloc(m * n, sizeof(double));
    check = cCompareAgainst(m, k, n, mA, mB);
    time2D(m, k, n, mA, mB, mC, bStar, 36, 6);
    compareCees(m, n, mC, check);
    free(check);

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

//Returns the Seqential C array of that size that is tested against for correctnesss
double *cCompareAgainst(int m, int k, int n, double mA[], double mB[]) {
    double *mC = calloc(m * n, sizeof(double));
    sequenMM(m, k, n, mA, mB, mC, 0);
    return mC;
}

void compareCees(int m, int n, double mC[], double testAgainst[]) {
    //most of this code is copied over from Assignment1
    int equal = 1;
    int value1;
    int value2;

    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            value1 = mC[i * n + j];
            value2 = testAgainst[i * n +j];
            if(value1 != value2){
                equal = 0;
            }
        }
    }

    if(equal == 1) {
        printf("\nThe arrays made by this style are correct\n");
    } else {
        printf("\nThe arrays made by this style and correct;\n");
    }

}