/** 
*   @file vhcall_demo.c
*   @brief This file will perform a selection operator with random generated int numbers  on a NEC Aurora Vector engine 
*   
*   
*
*	This file will be executed as main programm on the Vector engine
*	VH-API is used to call a dynamic library and run it on a XEON server. 
*	The lib fills an int array with random numbers. Then it returns the data to host
*	Host performances a selection Operator on the input data, calculates a result array and a hit rate.
*	
*	
*
*   @date 3/24/2019
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <libvhcall.h>
#include <omp.h>
#include "vh_lib.h"
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <libvhcall.h>
#ifdef __cplusplus
}
#endif





//function to get the current time in miliseconds
static inline long getTimestamp(){
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    // timestap is additive from 2 components: current time in nanoseconds + current time in seconds since epoch
    //tv_nsec only counts between 0 and 0,99 seconds. Therefore a timestampt needs to use tv_sec and tv_nsec
    return ts.tv_nsec/1000000 + ts.tv_sec*1000;
}


int
main (int argc, char **argv)
{
//variables used to measure the time needed for different functions
long start= getTimestamp();
long random_number_time; 
long selection_time;

//VH function handles, used to access lib functions.
int64_t symid2 = -1;
int datasize =200; //Number of elements to be randomly generated
uint32_t randoms[datasize];
uint32_t lower=5000;
uint32_t upper=10000;
uint32_t result[datasize]; //holds all result information. 1 if hit, 0 if miss. Type uint_32t to allow vectorization

 char cwd[PATH_MAX];
   if (getcwd(cwd, sizeof(cwd)) != NULL) 
       printf("Current working dir: %s\n", cwd);
   
      

	vhcall_handle h = vhcall_install("./vh_lib.so");
	if (h == (vhcall_handle)-1) {
        perror("vhcall_install");
		exit(1);
	}
	
	symid2 = vhcall_find(h, "generateInts");
	if (symid2 == -1) {
        perror("vhcall_args_alloc");
		exit(1);
	}
	
	printf("vhcall prepared\n\n");
	//struct for managing data transfers
	struct sendrecv_data sr;
	
	
	//Size of the buffer to copy is: size of randoms in byte (*4) + size of struct sr (32 bytes)
	long long buffersize = ((long long) datasize)*4 +32;
	float mbyte = ((float)datasize*4)/(1024.0*1024.0);
	printf( "buffersize: %lli  elements: %i  %f MBs\n",buffersize,datasize,mbyte);
	int * obuff;
	sr.addr = randoms;
	sr.size = sizeof(randoms);
	sr.result = malloc(sizeof(randoms));
	//sr.idata.push_back(1);
	//sr.test=11;
	
	long endsetup= getTimestamp();	
	long setup = endsetup -start;
	printf("Time for setup of VH API: %i ms \n",setup);

	if (vhcall_invoke(symid2, &sr, sizeof(sr), NULL, 0)) {
		perror("vhcall_invoke 2");
		exit(1);
	}
	int * erg = (int*)sr.result;
	printf("demo2 recv buffer on VE\n");
	
	random_number_time = getTimestamp() -endsetup;
	printf("Time for creating random array of ints: %i ms \n",random_number_time);
   long starts = getTimestamp();

   //do selection
	for( int i = 0; i < datasize; ++i ) {
      result[ i ] = ( erg[ i ] < upper ) & ( erg[ i ] > lower ) ;
   }
	long selection = getTimestamp() -starts;
	int hits=0;
	

for(int f=0;f<datasize;f++)
{
    if(result[f]){
    hits++;
    }
    
}


float phit = (float)hits / (float)datasize;

printf("hits: %i  percentage of hits: %f \n",hits,100*phit );
	printf("Time for selection: %i ms \n",selection);
float gbs = (mbyte/1024.0) / (selection /1000.0);
printf("Throughput: %f  [GB/s]\n",gbs);

	return 0;
}
