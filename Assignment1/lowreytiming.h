//A timing file modeled after the one given by the professor, but for c instead of cpp
#include <time.h>
#define LOWREYTIMING_H

//variable and struct declaration
struct timespec starting;
struct timespec ending;
struct timespec elapsed;
//int to test if running
int running;

//function declaration
void start();
void stop();
void elapsedTime();