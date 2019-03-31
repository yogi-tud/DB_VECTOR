/** 
*   @file selection_range.cpp 
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



int main(void)
{
long start= getTimestamp();
long random_number_time; 
long selection_time;
//it depends on the size if the calculation gets vectorized
//set the size of the input data, randomly generated
//autovectorize up to 131072 elements, which equals 4096kb of memory
int datasize = 200000000;
uint32_t data[datasize];
uint32_t result[datasize];

for( uint32_t i = 0; i < datasize; ++i ) {
      data[ i ]= rand() %10000;

   }

   



//generate random input data
//int * data = createIntArrayRandomNumbers(datasize );



//Selection calculation

//set up constants
 
    uint32_t lower = 5000;    
    uint32_t upper = 10000;
    

bool res;
int val;
int  *d2;
bool *r2;

random_number_time = getTimestamp() -start;
printf("Time for creating random array of ints: %i ms \n",random_number_time); 

start= getTimestamp();


//pointer to array element i * 256   inner loop should process only 256 elements, starting at i * 256 
//d2 = &data[i*256];
//r2 = &result[i*256];
 

 #pragma _NEC nofuse 
   for( uint32_t i = 0; i < datasize; ++i ) {
      result[ i ] = ( data[ i ] < upper ) & ( data[ i ] > lower ) ;
   }


int hits=0;

long end = getTimestamp();
long dif = end-start;
for(int f=0;f<datasize;f++)
{
    if(result[f]){
    hits++;
    }
    
}
float phit = (float)hits / (float)datasize;

printf("hits: %i  percentage of hits: %f %%\nTime needed for selection: %i ms \n",hits,100*phit, dif);


}
