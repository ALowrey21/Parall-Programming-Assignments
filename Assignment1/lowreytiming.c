#include <time.h>
#include "lowreytiming.h"
//A timing file modeled after the one given by the professor, but for c instead of cpp
/*
Structure of the timespec structure given in time.h for reference
struct timespec {
    time_t tv_sec;   // whole seconds
    long   tv_nsec;  // nanoseconds (0 to 999,999,999)
};
*/

struct timespec starting;
struct timespec ending;
struct timespec elapsed;
int running;

void start() {
    //ready set go!!
    clock_gettime(CLOCK_MONOTONIC, &starting);
    running = 1;
}

void stop() {
    //doesn't take a time if the clock is not running
    if(running == 1) {
        clock_gettime(CLOCK_MONOTONIC, &ending);
        running = 0;
    }
}

void elapsedTime() {
    //Get the times into one variable
    double starttime = starting.tv_sec + (starting.tv_nsec * 1e-9);
    double endtime = ending.tv_sec + (ending.tv_nsec * 1e-9);
    double totalTime = endtime - starttime;
    
    //put the values into a struct can then be used in the main file
    elapsed.tv_sec = (time_t) totalTime;
    elapsed.tv_nsec = (long)((totalTime - elapsed.tv_sec) * 1e9);

}