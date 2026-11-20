#include "assignment2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <omp.h>

//define constant values
#define MAKEG 1e9
//number of times that each test is run
#define REPEATCOUNT 11

//Declare files
FILE *ompSequentialRaw = NULL;
FILE *ompSequentialMedians = NULL;
FILE *ompOneDRaw = NULL;
FILE *ompOneDMedians = NULL;
FILE *ompTwoDRaw = NULL;
FILE *ompTwoDMedians = NULL;

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
    double median;
    double mean;
    double max;
    double min;
} timeStats;

//declare functions
void sequenMM(int m, int k, int n, double mA[], double mB[], double mC[], int b);
void oneDMM(int start, int end, int k, int n, double mA[], double mB[], double mC[], int b); 
void twoDMM(int rowStart, int rowEnd, int colStart, int colEnd, int n, int k, double mA[], double mB[], double mC[], int b);
timeStats timeSequential(int m, int k, int n, double mA[], double mB[], double mC[], int b);
void create1DSections(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p, oneDArgs sections[]);
void create2DSections(int m, int k, int n, double mA[], double mB[], double mC[], int b, int pr, int pc, twoDArgs sections[]);
timeStats findTimeStats(double times[]);
void matrixCReset(int m, int n, double matrix[]);
double *cCompareAgainst(int m, int k, int n, double mA[], double mB[]);
void compareCees(int m, int n, double mC[], double testAgainst[]);

int main(int argc, char *argv[]){
    //argc: number of arguments in argv
    //argv: a string of the values included in runing in the terminal
    //formats: 
    // Sequential: file mode m k n b
    // One-Dimension: file mode m k n b p
    // Two-Dimension: file mode m k n b pr pc
   
    if(argc < 6) {
        fprintf(stderr, "Input correct number of values please use mode m k n (b and (p or pr pc)) \n");
        return 1;
    }
    int mode = atoi(argv[1]);
    int m = atoi(argv[2]);
    int k = atoi(argv[3]);
    int n = atoi(argv[4]);
    int b = atoi(argv[5]);

    //create the arrays
    double *arrayA = generate_matrix(m, k, 1);
    double *arrayB = generate_matrix(k, n, 2);
    double *arrayC = calloc(m * n, sizeof(double));



    switch(mode) {
        case 0: {
            //open needed files
            ompSequentialRaw = fopen("data/omp_sequential_raw.csv", "a");
            if (ompSequentialRaw == NULL) {
                    fprintf(stderr, "Could not open data/omp_sequential_raw.csv\n");
                    return 1;
            }
            //If the file is empty, add a header
            fseek(ompSequentialRaw, 0, SEEK_END);
            if (ftell(ompSequentialRaw) == 0) {
                fprintf(ompSequentialRaw, "Attempt,M,K,N,b,Time,GFLOPS\n");
            }
            ompSequentialMedians = fopen("data/omp_sequential_medians.csv", "a");
            if (ompSequentialMedians == NULL) {
                fprintf(stderr, "Could not open data/omp_sequential_medians.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(ompSequentialMedians, 0, SEEK_END);
            if (ftell(ompSequentialMedians) == 0) {
                fprintf(ompSequentialMedians, "M,K,N,b,Median,Mean,Minimum,Maximum,Median_GFLOPS\n");
            }
            
            //If it is sequential, run it with the inputted B
            timeStats stats = timeSequential(m, k, n, arrayA, arrayB, arrayC, b);
            double median = stats.median;
            double mean = stats.mean;
            double min = stats.min;
            double max = stats.max;
            double rate = (double)2 * m * k * n / median;
            fprintf(ompSequentialMedians, "%d,%d,%d,%d,%f,%f,%f,%f,%f\n",m, k, n, b, median, mean, min, max, rate / MAKEG);
            
            fclose(ompSequentialMedians);
            fclose(ompSequentialRaw);
            break; 
//----------------------------------------------------------------------------------------------------------------------------------------------
        }
        case 1: {
            if(argc < 7) {
                fprintf(stderr, "Input correct number of values please use mode m k n b p\n");
                return 1;
            }
            int p = atoi(argv[6]);
            if(p < 1) {
                fprintf(stderr, "p was 0 or negative. Try again with only a positive value\n");
                return 1;
            }
            double times[REPEATCOUNT];
            omp_set_num_threads(p);
            oneDArgs sections[p];
            double *comparable = cCompareAgainst(m, k, n, arrayA, arrayB);
            create1DSections(m, k, n, arrayA, arrayB, arrayC, b, p, sections);

            //Open up needed files
            ompOneDRaw = fopen("data/omp_1D_raw.csv", "a");
            if (ompOneDRaw == NULL) {
                    fprintf(stderr, "Could not open data/omp_1D_raw.csv\n");
                    return 1;
            }
            //If the file is empty, add a header
            fseek(ompOneDRaw, 0, SEEK_END);
            if (ftell(ompOneDRaw) == 0) {
                fprintf(ompOneDRaw, "Attempt,M,K,N,b,p,Time,GFLOPS\n");
            }
            ompOneDMedians = fopen("data/omp_1D_medians.csv", "a");
            if (ompOneDMedians == NULL) {
                fprintf(stderr, "Could not open data/omp_1D_medians.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(ompOneDMedians, 0, SEEK_END);
            if (ftell(ompOneDMedians) == 0) {
                fprintf(ompOneDMedians, "M,K,N,b,p,Median,Mean,Minimum,Maximum,Median_GFLOPS\n");
            }
            //Warm - Up
            matrixCReset(m, n, arrayC);
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < p; i++) {
                int start = sections[i].start;
                int end = sections[i].end;
                oneDMM(start, end, k, n, arrayA, arrayB, arrayC, b); 
            }
            compareCees(m, n, arrayC, comparable);
            for (int i = 0; i < REPEATCOUNT; i++) {
                matrixCReset(m, n, arrayC);
                double startTime = omp_get_wtime();
                #pragma omp parallel for schedule(static)
                for(int j = 0; j < p; j++) {
                    int start = sections[j].start;
                    int end = sections[j].end;
                    oneDMM(start, end, k, n, arrayA, arrayB, arrayC, b);
                }
                double endTime = omp_get_wtime();
                double timing = endTime - startTime;
                double rate = (double) 2 * m * k * n / timing;
                printf("\nOne-Dimensional Attempt %d with b = %d and p = %d\n", i + 1, b, p);
                printf("Time: %f\n", timing);
                times[i] = timing;
                fprintf(ompOneDRaw, "%d,%d,%d,%d,%d,%d,%f,%f\n", i + 1, m, k, n, b, p, timing, rate / MAKEG);
            }
            compareCees(m, n, arrayC, comparable);
            timeStats stats = findTimeStats(times);
            double median = stats.median;
            double mean = stats.mean;
            double min = stats.min;
            double max = stats.max;
            double rate = (double)2 * m * k * n / median;
            fprintf(ompOneDMedians, "%d,%d,%d,%d,%d,%f,%f,%f,%f,%f\n", m, k, n, b, p, median, mean, min, max, rate / MAKEG);

            fclose(ompOneDMedians);
            fclose(ompOneDRaw);
            free(comparable);
            break;
//----------------------------------------------------------------------------------------------------------------------------------------------
        }
        case 2: {
            if(argc < 8) {
                fprintf(stderr, "Input correct number of values please use mode m k n b pr pc\n");
                return 1;
            }
            int pr = atoi(argv[6]);
            int pc = atoi(argv[7]);
            int p = pr * pc;
            if(pr < 1 || pc < 1) {
                fprintf(stderr, "pr or pc is negative. Please use positive values and follow the format of 2 m k n b pr pc\n");
                return 1;
            }
            double times[REPEATCOUNT];
            omp_set_num_threads(p);
            twoDArgs sections[p];
            double *comparable = cCompareAgainst(m, k, n, arrayA, arrayB);
            create2DSections(m, k, n, arrayA, arrayB, arrayC, b, pr, pc, sections);

            //Open up needed files
            ompTwoDRaw = fopen("data/omp_2D_raw.csv", "a");
            if (ompTwoDRaw == NULL) {
                    fprintf(stderr, "Could not open data/omp_2D_raw.csv\n");
                    return 1;
            }
            //If the file is empty, add a header
            fseek(ompTwoDRaw, 0, SEEK_END);
            if (ftell(ompTwoDRaw) == 0) {
                fprintf(ompTwoDRaw, "Attempt,M,K,N,b,p,pr,pc,Time,GFLOPS\n");
            }
            ompTwoDMedians = fopen("data/omp_2D_medians.csv", "a");
            if (ompTwoDMedians == NULL) {
                fprintf(stderr, "Could not open data/omp_2D_medians.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(ompTwoDMedians, 0, SEEK_END);
            if (ftell(ompTwoDMedians) == 0) {
                fprintf(ompTwoDMedians, "M,K,N,b,p,pr,pc,Median,Mean,Minimum,Maximum,Median_GFLOPS\n");
            }

                        //Warm - Up
            matrixCReset(m, n, arrayC);
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < p; i++) {
                int rowStart = sections[i].rowStart;
                int rowEnd = sections[i].rowEnd;
                int colStart = sections[i].colStart;
                int colEnd = sections[i].colEnd;
                twoDMM(rowStart, rowEnd, colStart, colEnd, n, k, arrayA, arrayB, arrayC, b);
            }
            compareCees(m, n, arrayC, comparable);
            for (int i = 0; i < REPEATCOUNT; i++) {
                matrixCReset(m, n, arrayC);
                double startTime = omp_get_wtime();
                #pragma omp parallel for schedule(static)
                for (int j = 0; j < p; j++) {
                    int rowStart = sections[j].rowStart;
                    int rowEnd = sections[j].rowEnd;
                    int colStart = sections[j].colStart;
                    int colEnd = sections[j].colEnd;
                    twoDMM(rowStart, rowEnd, colStart, colEnd, n, k, arrayA, arrayB, arrayC, b);
                }
                double endTime = omp_get_wtime();
                double timing = endTime - startTime;
                double rate = (double) 2 * m * k * n / timing;
                printf("\nTwo-Dimensional Attempt %d with b = %d and pr = %d, pc = %d\n", i + 1, b, pr, pc);
                printf("Time: %f\n", timing);
                times[i] = timing;
                fprintf(ompTwoDRaw, "%d,%d,%d,%d,%d,%d,%d,%d,%f,%f\n", i + 1, m, k, n, b, p, pr, pc, timing, rate / MAKEG);
            }
            compareCees(m, n, arrayC, comparable);
            timeStats stats = findTimeStats(times);
            double median = stats.median;
            double mean = stats.mean;
            double min = stats.min;
            double max = stats.max;
            double rate = (double)2 * m * k * n / median;
            fprintf(ompTwoDMedians, "%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f\n", m, k, n, b, p, pr, pc,median, mean, min, max, rate / MAKEG);

            free(comparable);
            fclose(ompTwoDMedians);
            fclose(ompTwoDRaw);
            break;
//----------------------------------------------------------------------------------------------------------------------------------------------
        } default: {
            fprintf(stderr, "Input correct number of values please: \n");
            fprintf(stderr, "Sequential: file mode m k n b\n");
            fprintf(stderr, "One-Dimension: file mode m k n b p\n");
            fprintf(stderr, "Two-Dimension: file mode m k n b pr pc\n");

            return 1;

        }
    }
    //free arrays
    free(arrayA);
    free(arrayB);
    free(arrayC);
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


//Timing for the sequential algorithm
timeStats timeSequential(int m, int k, int n, double mA[], double mB[], double mC[], int b) {
    double times[REPEATCOUNT];
    double *comparable = cCompareAgainst(m, k, n, mA, mB);
    //one run before timing
    matrixCReset(m, n, mC);
    sequenMM(m, k, n, mA, mB, mC, b);
    compareCees(m, n, mC, comparable);

    //now for timing:
    for (int i = 0; i < REPEATCOUNT; i++) {
        matrixCReset(m, n, mC);
        double startTime = omp_get_wtime();
        sequenMM(m, k, n, mA, mB, mC, b);
        double endTime = omp_get_wtime();
        double timing = endTime - startTime;
        printf("\nSequential Attempt %d with b = %d\n", i + 1 , b);
        printf("Time: %f\n", timing);
        double rate = (double)2 * m * k * n / timing;
        fprintf(ompSequentialRaw, "%d,%d,%d,%d,%d,%f,%f\n", i + 1, m, k, n, b, timing, rate / MAKEG);
        times[i] = timing;
    }
    compareCees(m, n, mC, comparable);
    free(comparable);
    return findTimeStats(times);
}

//Create the sections that each thread will run over for one-dimension
void create1DSections(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p, oneDArgs sections[]) {
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
}

void create2DSections(int m, int k, int n, double mA[], double mB[], double mC[], int b, int pr, int pc, twoDArgs sections[]) {
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
}

timeStats findTimeStats(double times[]) {
    timeStats stats;
    //Sort the times from least to greatest
    for(int i = 0; i < REPEATCOUNT; i++) {
        double current = times[i];
        int j = i - 1;

        while(j >= 0 && times[j] > current) {
            times[j+1] = times[j];
            j--;
        }
        times[j + 1] = current;
    }
    stats.median = times[REPEATCOUNT / 2];
    stats.max = times[REPEATCOUNT - 1];
    stats.min = times[0];
    double sum = 0;
    for(int i = 0; i < REPEATCOUNT; i++) {
        sum += times[i];
    }
    stats.mean = sum / REPEATCOUNT;
    return stats;
 
}

//CReset Function copied from assignment1
void matrixCReset(int m, int n, double matrix[]) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            matrix[i * n + j] = 0;
        }
    }
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
    double value1;
    double value2;

    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            value1 = mC[i * n + j];
            value2 = testAgainst[i * n +j];
            double difference = fabs(value1 - value2);
            double scale = fmax(fabs(value1), fabs(value2));
            if(difference > scale * 1e-9){
                equal = 0;
            }
        }
    }

    if(equal == 1) {
        printf("\nThe arrays made by this style are correct\n");
    } else {
        printf("\nThe arrays made by this style are NOT correct;\n");
    }

}