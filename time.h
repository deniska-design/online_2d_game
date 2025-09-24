#ifndef TIME_MODULO
#define TIME_MODULO

#include "stdlib.h"
#include <random>
#include <time.h>

using namespace std;

bool stopwatch(time_t WaitingTime, time_t CurrentTime)
{
    static time_t EndTime;
    static bool firstExecution = true;

    if (firstExecution)
    {
        EndTime = WaitingTime + CurrentTime;
        firstExecution = false;
    }
    if (!firstExecution)
    {
        if (time(NULL) >= EndTime)
        {
            firstExecution = true;
            return true;
        }
    }
    return false;
}

float Random(float a, float b)
{    
    srand(time(NULL));
    static mt19937 gen(random_device{}()); 
    uniform_real_distribution<float> dis(a,b);
    return dis(gen);
}

#endif