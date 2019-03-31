/** 
*   @file intrinsic.cpp 
*   @brief This file will perform a Range selection operator with random generated int numbers  on a NEC Aurora Vector engine 
*   for each value in the array data, the program calculates if it is a hit or a miss. 
*   a corresponding result array gets filled with 1 and 0s. 1 means a hit and 0 a miss.
*
*
*   @author Johannes Fett
*
*   @date 1/16/2019
*/


#include <limits.h> 
#include <stdio.h>                                                                                                                               
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>

#define NIL INT_MIN



//function to get the current time in miliseconds
static inline long getTimestamp(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    // timestap is additive from 2 components: current time in nanoseconds + current time in seconds since epoch
    //tv_nsec only counts between 0 and 0,99 seconds. Therefore a timestampt needs to use tv_sec and tv_nsec
    return ts.tv_nsec/1000000 + ts.tv_sec*1000;
}

int main()
{
int datasize= 10;
    Vector<uint_32> data(datasize);

printf("Intrinsic test\n");

}
