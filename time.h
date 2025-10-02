#ifndef TIME_MODULO
#define TIME_MODULO

#include "stdlib.h"
#include <random>
#include <time.h>

using namespace std;

bool stopwatch(time_t WaitingTime, time_t CurrentTime, int Number)
{
    time_t EndTime[2];
    static time_t Curtim[2];
    static bool firstExecution[2] = {true, true};

    if (firstExecution[Number])
    {
        Curtim[Number] = CurrentTime;
        firstExecution[Number]  = false;
    }
    if (!firstExecution[Number])
    {
        EndTime[Number] = WaitingTime + Curtim[Number];
        if (CurrentTime >= EndTime[Number])
        {
            firstExecution[Number] = true;
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