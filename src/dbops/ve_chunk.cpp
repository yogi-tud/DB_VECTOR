//
// Created by johannes on 28.06.19.
//

#include <stddef.h>
#include <stdlib.h>

#include <stdio.h>
#include <iostream>
#include "stopwatch.h"
#include "memory.h"
#include <libvhcall.h>
#ifdef __cplusplus
extern "C" {
#endif

extern vhcall_args* vhcall_args_alloc(void);
#ifdef __cplusplus
}
#endif
using namespace std;

uint64_t invokevh(uint64_t * data, uint64_t * result, uint64_t datacount)
{
    size_t size= datacount * sizeof(uint64_t);
    int64_t symid;
    uint64_t retval = -1;
    vhcall_handle handle;
    vhcall_args *ca;
    handle = vhcall_install("./vh_omp.so");    //load shared lib
    //handle = vhcall_install("./sync_vh.so");    //load shared lib
    symid = vhcall_find(handle, "compress");   //load compress function
    ca = vhcall_args_alloc();   //argument object for vhcall function arguments
    vhcall_args_set_veoshandle(ca, 0);  //set veos handle as argument
    vhcall_args_set_pointer(ca, VHCALL_INTENT_IN, 1, (uint64_t *) data,
                            size);  //set Select input data as function argument
    vhcall_args_set_pointer(ca, VHCALL_INTENT_INOUT, 2, (uint64_t *) result,
                            size); //set select result data as function argument
    vhcall_args_set_u64(ca, 3, datacount);
    vhcall_invoke_with_args(symid, ca, &retval);    //call VH function with arguments
    vhcall_args_free(ca);
    return retval;
}

void printcmp (uint64_t * result)
{
    std::cout<<"[VE] RETURNED COMRPESS DATA: "<<std::endl;
    for(int i=0;i<10;i++){

        std::cout<<result[i]<<" ";
    }
    std::cout<<std::endl;
}



void randomnumgen(uint64_t * data, size_t datacount)
{
    for(uint64_t i = 0; i < datacount; ++i ) {
        data[ i ]= rand() %10000;

    }
}

int select (uint64_t * data, uint64_t * result, size_t datacount)
{

    uint32_t lower = 5000;
    uint32_t upper = 10000;

#pragma _NEC nofuse
    for(uint64_t i = 0; i < datacount; ++i ) {
        result[ i] = ( data[ i ] < upper ) & ( data[ i ] > lower ) ;
    }



    int hits=0;


    for(int f=0; f < datacount; f++)
    {
        if(result[f]){
            hits++;
        }

    }
   return hits;

}

int main() {
    WallClockStopWatch sw;
    double dsel;
    size_t datacount =67108864;
    size_t datasize = datacount *sizeof(uint64_t);
    size_t chunks = 2;
    size_t chunkcount = datacount/chunks;

    uint64_t * data = (uint64_t *) aligned_alloc(64,datasize+1);
    uint64_t * result=intel_malloc< uint64_t >( datacount +1);



    randomnumgen(data, datacount);

    sw.start();
    for (int k =0;k<chunks;k++)
    {
        select(data, result, chunkcount);
        invokevh(data, result, chunkcount);
        data+= chunkcount;
        result += chunkcount;
    }
    sw.stop();
    dsel = sw.duration();
    cout<<"datasize "<<datacount* sizeof(uint64_t)/(1024*1024)<<" MB"<<" chunks "<<chunks<<endl;

    cout<<"TOTAL RUNTIME SELECT + COMPRESS: "<<dsel<<endl;




    return 0;
}


