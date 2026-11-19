#include "assignment2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

//define constant values
#define MAKEG 1e9
//number of times that each test is run
#define REPEATCOUNT 11


//declare files
FILE *pthSequentialRaw = NULL;
FILE *pthSequentialMedians = NULL;
FILE *pthOneDRaw = NULL;
FILE *pthOneDMedians = NULL;
FILE *pthTwoDRaw = NULL;
FILE *pthTwoDMedians = NULL;
FILE *bStarHolder = NULL;

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

//declartion of functions
void sequenMM(int m, int k, int n, double mA[], double mB[], double mC[], int b);
void oneDMM(int start, int end, int k, int n, double mA[], double mB[], double mC[], int b);
void twoDMM(int rowStart, int rowEnd, int colStart, int colEnd, int n, int k, double mA[], double mB[], double mC[], int b);

timeStats timeSequential(int m, int k, int n, double mA[], double mB[], double mC[], int b);
void create1DSections(int m, int k, int n, double mA[], double mB[], double mC[], int b, int p, oneDArgs sections[]);
void create2DSections(int m, int k, int n, double mA[], double mB[], double mC[], int b, int pr, int pc, twoDArgs sections[]);



timeStats findTimeStats(double times[]);
void matrixCReset(int m, int n, double matrix[]);
void *oneDWrapper(void *arg);
void *twoDWrapper(void *arg);
double *cCompareAgainst(int m, int k, int n, double mA[], double mB[]);
void compareCees(int m, int n, double mC[], double testAgainst[]);


int main(int argc, char *argv[]){
    //argc: number of arguments in argv
    //argv: a string of the values included in runing in the terminal
    //formats: 
    // Sequential file type m k n
    // One-Dimension file type m k n b p
    // Two-Dimension file type m k n b pr pc
    //vaild types
    if(argc < 5) {
        fprintf(stderr, "Input correct number of values please use mode m k n (b and (p or pr pc)) \n");
        return 1;
    }
    int mode = atoi(argv[1]);
    int m = atoi(argv[2]);
    int k = atoi(argv[3]);
    int n = atoi(argv[4]);
    
    //create the arrays
    double *arrayA = generate_matrix(m, k, 1);
    double *arrayB = generate_matrix(k, n, 2);
    double *arrayC = calloc(m * n, sizeof(double));
    
    //decides what functions needs to be run
//----------------------------------------------------------------------------------------------------------------------------------------------
    switch (mode) {
        case 0: {
            //sequential
            //open needed files
            pthSequentialRaw = fopen("data/pth_sequential_raw.csv", "a");
            if (pthSequentialRaw == NULL) {
                fprintf(stderr, "Could not open data/pth_sequential_raw.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(pthSequentialRaw, 0, SEEK_END);
            if (ftell(pthSequentialRaw) == 0) {
                fprintf(pthSequentialRaw, "Attempt,M,K,N,b,Time,GFLOPS\n");
            }

            pthSequentialMedians = fopen("data/pth_sequential_medians.csv", "a");
            if (pthSequentialMedians == NULL) {
                fprintf(stderr, "Could not open data/pth_sequential_medians.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(pthSequentialMedians, 0, SEEK_END);
            if (ftell(pthSequentialMedians) == 0) {
                fprintf(pthSequentialMedians, "M,K,N,b,Median,Mean,Minimum,Maximum,Median_GFLOPS\n");
            }

            //create the list of bees that will be used to find bStar
            int bees[6] = {0, 16, 32, 64, 128, 256};
            //compare median times against each other using times[i]
            double times[6];
            //only run through all of the bees if M=K=N=2048
            if(m == 2048 && k == 2048 && n == 2048) {
                for(int i = 0; i < 6; i++) {
                    timeStats stats = timeSequential(m, k, n, arrayA, arrayB, arrayC, bees[i]);
                    double median = stats.median;
                    double mean = stats.mean;
                    double min = stats.min;
                    double max = stats.max;
                    double rate = (double)2 * m * k * n / median;
                    times[i] = median;
                    fprintf(pthSequentialMedians, "%d,%d,%d,%d,%f,%f,%f,%f,%f\n",m, k, n, bees[i], median, mean, min, max, rate / MAKEG);
                }

                double smallTime = INFINITY;
                int smallB = -1;
                for(int i = 0; i < 6; i++) {
                    if(times[i] < smallTime) {
                        smallTime = times[i];
                        smallB = bees[i];
                    }
                }
                printf("\nThe b with the smallest median time is %d with a time of %f\n", smallB, smallTime);

                //get bStar
                bStarHolder = fopen("data/b_star_holder.txt", "w");
                if (bStarHolder == NULL) {
                    fprintf(stderr, "Could not open data/b_star_holder.txt\n");
                    return 1;
                }
                fprintf(bStarHolder, "%d\n", smallB);
                fclose(bStarHolder);
            } else {
                timeStats stats = timeSequential(m, k, n, arrayA, arrayB, arrayC, 0);
                double median = stats.median;
                double mean = stats.mean;
                double min = stats.min;
                double max = stats.max;
                double rate = (double)2 * m * k * n / median;
                fprintf(pthSequentialMedians, "%d,%d,%d,%d,%f,%f,%f,%f,%f\n",m, k, n, 0, median, mean, min, max, rate / MAKEG);

            }
            fclose(pthSequentialRaw);
            fclose(pthSequentialMedians);
            break;
//----------------------------------------------------------------------------------------------------------------------------------------------
        }
        case 1: {
            //one-dimensional
            if(argc < 7) {
                fprintf(stderr, "Input correct number of values please. Use format 1 m k n b p\n");
                return 1;
            }
            int b = atoi(argv[5]);
            int p = atoi(argv[6]);
            double times[REPEATCOUNT];
            pthread_t threads[p];
            double *comparable = cCompareAgainst(m, k, n, arrayA, arrayB);

            if(p < 1) {
                fprintf(stderr, "p was 0 or negative. Try again with only a positive value");
                return 1;
            }
            //open needed files
            pthOneDRaw = fopen("data/pth_1D_raw.csv", "a");
            if (pthOneDRaw == NULL) {
                fprintf(stderr, "Could not open data/pth_1D_raw.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(pthOneDRaw, 0, SEEK_END);
            if (ftell(pthOneDRaw) == 0) {
                fprintf(pthOneDRaw, "Attempt,M,K,N,b,p,Time,GFLOPS\n");
            }

            pthOneDMedians = fopen("data/pth_1D_medians.csv", "a");
            if (pthOneDMedians == NULL) {
                fprintf(stderr, "Could not open data/pth_1D_medians.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(pthOneDMedians, 0, SEEK_END);
            if (ftell(pthOneDMedians) == 0) {
                fprintf(pthOneDMedians, "M,K,N,b,p,Median,Mean,Minimum,Maximum,Median_GFLOPS\n");
            }

            //set up the sections that each thread will run over
            oneDArgs sections[p];
            create1DSections(m, k, n, arrayA, arrayB, arrayC, b, p, sections);
            
            //Warm-Up run
            matrixCReset(m, n, arrayC);
            for (int j = 0; j < p; j++) {
                pthread_create(&threads[j], NULL, oneDWrapper, &sections[j]);
            }
            for (int j = 0; j < p; j++) {
                pthread_join(threads[j], NULL);
            }
            compareCees(m, n, arrayC, comparable);

            //create threads and run through
            for(int i = 0; i < REPEATCOUNT; i++) {
                matrixCReset(m, n, arrayC);
                double timeStart = now_seconds();
                for (int j = 0; j < p; j++) {
                    pthread_create(&threads[j], NULL, oneDWrapper, &sections[j]);
                }
                for (int j = 0; j < p; j++) {
                    pthread_join(threads[j], NULL);
                }
                double timeEnd = now_seconds();
                double timing = timeEnd - timeStart;
                double rate = (double) 2 * m * k * n / timing;
                printf("\nOne-Dimensional Attempt %d with b = %d and p = %d\n", i + 1, b, p);
                printf("Time: %f\n", timing);
                times[i] = timing;
                fprintf(pthOneDRaw, "%d,%d,%d,%d,%d,%d,%f,%f\n", i + 1, m, k, n, b, p, timing, rate / MAKEG);
            }
            compareCees(m, n, arrayC, comparable);
            timeStats stats = findTimeStats(times);
            double median = stats.median;
            double mean = stats.mean;
            double min = stats.min;
            double max = stats.max;
            double rate = (double)2 * m * k * n / median;
            fprintf(pthOneDMedians, "%d,%d,%d,%d,%d,%f,%f,%f,%f,%f\n", m, k, n, b, p, median, mean, min, max, rate / MAKEG);

            free(comparable);
            fclose(pthOneDMedians);
            fclose(pthOneDRaw);
            break;
//----------------------------------------------------------------------------------------------------------------------------------------------
        }
        case 2: {
            //two-dimensional
            if(argc < 8) {
                fprintf(stderr, "Input correct number of values please\n");
                return 1;
            }
            int b = atoi(argv[5]);
            int pr = atoi(argv[6]);
            int pc = atoi(argv[7]);
            if(pr < 1 || pc < 1) {
                fprintf(stderr, "pr or pc was 0 or negative. Try again with only positive values");
                return 1;
            }
            int p = pr * pc;
            double times[REPEATCOUNT];
            pthread_t threads[p];
            twoDArgs sections[p];
            double *comparable = cCompareAgainst(m, k, n, arrayA, arrayB);

            //open up needed files
            pthTwoDRaw = fopen("data/pth_2D_raw.csv", "a");
            if (pthTwoDRaw == NULL) {
                fprintf(stderr, "Could not open data/pth_2D_raw.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(pthTwoDRaw, 0, SEEK_END);
            if (ftell(pthTwoDRaw) == 0) {
                fprintf(pthTwoDRaw, "Attempt,M,K,N,b,p,pr,pc,Time,GFLOPS\n");
            }

            pthTwoDMedians = fopen("data/pth_2D_medians.csv", "a");
            if (pthTwoDMedians == NULL) {
                fprintf(stderr, "Could not open data/pth_2D_medians.csv\n");
                return 1;
            }
            //If the file is empty, add a header
            fseek(pthTwoDMedians, 0, SEEK_END);
            if (ftell(pthTwoDMedians) == 0) {
                fprintf(pthTwoDMedians, "M,K,N,b,p,pr,pc,Median,Mean,Minimum,Maximum,Median_GFLOPS\n");
            }

            //set up sections that each thread will run over
            create2DSections(m, k, n, arrayA, arrayB, arrayC, b, pr, pc, sections);

            //Warm-Up run
            matrixCReset(m, n, arrayC);
            for(int j = 0; j < p; j++) {
                pthread_create(&threads[j], NULL, twoDWrapper, &sections[j]);
            }
            for(int j = 0; j < p; j++) {
                pthread_join(threads[j], NULL);
            }
            compareCees(m, n, arrayC, comparable);

            //Full timed runs
            for(int i = 0; i < REPEATCOUNT; i++) {
                matrixCReset(m, n, arrayC);
                double timeStart = now_seconds();
                for(int j = 0; j < p; j++) {
                    pthread_create(&threads[j], NULL, twoDWrapper, &sections[j]);
                }
                for(int j = 0; j < p; j++) {
                    pthread_join(threads[j], NULL);
                }
                double timeEnd = now_seconds();
                double timing = timeEnd - timeStart;
                double rate = (double) 2 * m * k * n / timing;
                printf("\nTwo-Dimension Attempt: %d with b: %d and p: %d\n", i + 1, b, p);
                printf("Dimensions of p are %d x %d\n", pr, pc);
                printf("Time: %f\n", timing);
                times[i] = timing;
                fprintf(pthTwoDRaw, "%d,%d,%d,%d,%d,%d,%d,%d,%f,%f\n", i + 1, m, k, n, b, p, pr, pc, timing, rate / MAKEG);
            }
            compareCees(m, n, arrayC, comparable);
            
            timeStats stats = findTimeStats(times);
            double median = stats.median;
            double mean = stats.mean;
            double min = stats.min;
            double max = stats.max;
            double rate = (double)2 * m * k * n / median;
            fprintf(pthTwoDMedians, "%d,%d,%d,%d,%d,%d,%d,%f,%f,%f,%f,%f\n", m, k, n, b, p, pr, pc, median, mean, min, max, rate / MAKEG);

            free(comparable);
            fclose(pthTwoDMedians);
            fclose(pthTwoDRaw);
            break;
//----------------------------------------------------------------------------------------------------------------------------------------------
        }
        default: {
            fprintf(stderr, "Please input what mode you want to use correctly\n");
            return 1;
        }
    }

    //freeing arrays
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
        double startTime = now_seconds();
        sequenMM(m, k, n, mA, mB, mC, b);
        double endTime = now_seconds();
        double timing = endTime - startTime;
        printf("\nSequential Attempt %d with b = %d\n", i + 1 , b);
        printf("Time: %f\n", timing);
        double rate = (double)2 * m * k * n / timing;
        fprintf(pthSequentialRaw, "%d,%d,%d,%d,%d,%f,%f\n", i + 1, m, k, n, b, timing, rate / MAKEG);
        times[i] = timing;
    }
    compareCees(m, n, mC, comparable);
    free(comparable);
    return findTimeStats(times);
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

//find the median, mean, max, and min times
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