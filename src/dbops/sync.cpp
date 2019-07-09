//
// Created by johannes on 28.06.19.
//

#include <stddef.h>
#include <stdlib.h>
#include <libvhcall.h>
#include <stdio.h>
#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif
extern vhcall_args* vhcall_args_alloc(void);
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

int main() {
    long start= getTimestamp();
    long random_number_time;
    long selection_time;
//it depends on the size if the calculation gets vectorized
//set the size of the input data, randomly generated
//autovectorize up to 131072 elements, which equals 4096kb of memory
    int datasize = 256;
    uint64_t data[datasize];
    uint64_t result[datasize];

    for( uint64_t i = 0; i < datasize; ++i ) {
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
        result[ i] = ( data[ i ] < upper ) & ( data[ i ] > lower ) ;
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


    //SET UP VHCALL API
    int64_t symid;
    vhcall_handle handle;
    vhcall_args *ca;
    uint64_t retval = -1;



    size_t size=256*sizeof(uint64_t);
    size_t size2=256*sizeof(uint64_t);
    handle = vhcall_install("./sync_vh.so");
    symid  = vhcall_find(handle, "compress");
    ca = vhcall_args_alloc();   //argument object for vhcall function arguments
    vhcall_args_set_veoshandle(ca, 2);  //set veos handle as argument
    vhcall_args_set_pointer(ca,  VHCALL_INTENT_IN, 0, (uint64_t *)data, size2);  //set Select input data as function argument
    vhcall_args_set_pointer(ca,  VHCALL_INTENT_IN, 1, (uint64_t *)result, size2); //2. buffer. breaks vhcall
    vhcall_invoke_with_args(symid, ca, &retval);    //call VH function with arguments

    vhcall_args_free(ca);
    return 0;
}


