/**
*   @file sh_mem.cpp
*   @brief Testing auroras shared memory. reads from shared memory, created by another process
*
*
*   @author Johannes Fett
*
*   @date 2/4/2019
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include<iostream>
#include <unistd.h>
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

int memget(size_t datasize, key_t key)
{
    int shm_id=0;
    int err =0;
    shm_id = shmget(key, datasize, err);
    if (shm_id < 0) {
        printf("shmget error: %i\n %s ",err,strerror(errno));

    }
    return shm_id;
}

int main()
{
    size_t datacount =67108864;
    size_t datasize = datacount *sizeof(uint64_t);
    size_t chunks = 2;
    size_t chunkdatacount = datacount /chunks;
    size_t chunksize = datasize /chunks;
    size_t signalarraysize = chunks * sizeof(uint64_t);




    int shm_id_data=0;
    int shm_id_result=0;
    int shm_id_cmpdone=0;
    int shm_id_insig=0;

    key_t key_data = 9988;
    key_t key_result = 9987;
    key_t key_cmpdone = 9986;
    key_t key_insig = 9985;

    shm_id_data=memget(datasize, key_data);
    shm_id_result=memget(datasize, key_result);
    shm_id_cmpdone=memget(signalarraysize, key_cmpdone);
    shm_id_insig=memget(signalarraysize, key_insig);

    uint64_t * insig = (uint64_t *) shmat(shm_id_insig,(void*)0,0);
    uint64_t * data = (uint64_t *) shmat(shm_id_data,(void*)0,0);
    uint64_t * res = (uint64_t *) shmat(shm_id_result,(void*)0,0);
    uint64_t * cmpdone = (uint64_t *) shmat(shm_id_cmpdone,(void*)0,0);


    uint64_t * startpointer= res;




    int chunkctrl = 0;
    uint64_t cmptotal=0;
    //check for new input. if new input, make a VHCALL and write back results to res
    //also output signal that it is done
    cout<<"WAITING FOR SELECT DATA"<<endl;

        while (chunkctrl < chunks)
        {

        if(insig[chunkctrl]==1)
        {
            uint64_t cmpcount=0;

            cmpcount= invokevh(data, res, chunkdatacount);

            chunkctrl++;
            cmpdone[chunkctrl]=1;

            //pointer adjustment after calling vhcall

            data+= cmpcount;
            res += cmpcount;
            cmptotal+=cmpcount;
        }
            nanosleep((const struct timespec[]){{0, 500L}}, NULL);
        }
        cmpdone[0]=cmptotal;
        cout<<"ALL VHCALLS DONE"<<endl;
/*
    std::cout<<"[VE] RETURNED COMRPESS DATA: "<<std::endl;
    for(int i=0;i<cmptotal;i++){

        std::cout<<startpointer[i]<<" ";
    }
    std::cout<<std::endl;
*/
    //detach from shared memory

    shmdt(data);
    shmdt(res);
    shmdt(insig);
    shmdt(cmpdone);


}
