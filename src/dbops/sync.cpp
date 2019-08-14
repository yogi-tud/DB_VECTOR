//
// Created by johannes on 28.06.19.
//

#include <stddef.h>
#include <stdlib.h>
#include <libvhcall.h>
#include <stdio.h>
#include <iostream>
#include "stopwatch.h"
#include "memory.h"
#ifdef __cplusplus
extern "C" {
#endif
extern vhcall_args* vhcall_args_alloc(void);
#ifdef __cplusplus
}
#endif


int main() {


    size_t datasize =   1048576;
    std::cout<<"[VE] datacount: "<<datasize<<"\n";
    WallClockStopWatch sw;



//it depends on the size if the calculation gets vectorized
//set the size of the input data, randomly generated
//autovectorize up to 131072 elements, which equals 4096kb of memory

    //std::cout<<"[VE] DATASIZE: "<<datasize<<std::endl;
    sw.start();



     uint64_t* data = (uint64_t *) aligned_alloc(64,datasize*sizeof(uint64_t));
     uint64_t * result=intel_malloc< uint64_t >( datasize );



    for( uint64_t i = 0; i < datasize; ++i ) {
        data[ i ]= rand() %10000;

    }
    sw.stop();
    double d_rand = sw.duration();

    sw.reset();



//Selection calculation

//set up constants

    uint32_t lower = 5000;
    uint32_t upper = 10000;


    bool res;
    int val;
    int  *d2;
    bool *r2;









    sw.start();
#pragma _NEC nofuse
    for( uint64_t i = 0; i < datasize; ++i ) {
        result[ i] = ( data[ i ] < upper ) & ( data[ i ] > lower ) ;
    }



    int hits=0;


    for(int f=0;f<datasize;f++)
    {
        if(result[f]){
            hits++;
        }

    }
    float phit = (float)hits / (float)datasize;
    sw.stop();
    double d_sel = sw.duration();

    std::cout << "[VE] time for random num gen: "<<d_rand << "\n";
    std::cout << "[VE] time for selection: "<<d_sel << "\n";


    //SET UP VHCALL API
    sw.reset();
    sw.start();
    int64_t symid;
    vhcall_handle handle;
    vhcall_args *ca;
    uint64_t retval = -1;


    //std::cout<<"align of input in VE: "<<alignof((data))<<std::endl;
    //std::cout<<"align of res in VE: "<<alignof((result))<<std::endl;


    size_t size=datasize*sizeof(uint64_t);











    handle = vhcall_install("./sync_vh.so");    //load shared lib
    symid  = vhcall_find(handle, "compress");   //load compress function

    ca = vhcall_args_alloc();   //argument object for vhcall function arguments
    vhcall_args_set_veoshandle(ca, 0);  //set veos handle as argument
    vhcall_args_set_pointer(ca,  VHCALL_INTENT_IN, 1, (uint64_t *)data, size);  //set Select input data as function argument
    vhcall_args_set_pointer(ca,  VHCALL_INTENT_INOUT, 2, (uint64_t *)result, size); //set select result data as function argument
    vhcall_args_set_u64 (ca, 3, datasize);



    vhcall_invoke_with_args(symid, ca, &retval);    //call VH function with arguments
    sw.stop();
    double d_vhcall = sw.duration();
    std::cout<<"[VE] Time for doing compress:  "<<d_vhcall<<std::endl;

   //std::cout<<"[VE] RETURNED COMRPESS COUNT: "<<retval<<std::endl;
    std::cout<<"[VE] RETURNED COMRPESS DATA: "<<std::endl;
    for(int i=0;i<10;i++){

       std::cout<<result[i]<<" ";
    }
   std::cout<<std::endl;

    //



    vhcall_args_free(ca);
    return 0;
}


