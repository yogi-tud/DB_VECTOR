//
// Created by johannes on 24.08.19.
//

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include<iostream>
#include "stopwatch.h"
using namespace std;

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
/**
 * This function performs a selection operator and writes the result back into a shared memory buffer
 * @param data
 * @param result
 * @param datacount
 * @param shm_id
 * @return
 */
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

int reservershm(size_t datasize, key_t key)
{
    int shm_id;
    int err;


//create shared memory.
    shm_id = shmget(key, datasize, IPC_CREAT|0666);

//shm_id is -1 in case shmget fails to allocate shared memory
    if (shm_id < 0) {
        printf("shmget error: %i\n %s ",err,strerror(errno));

    }
    printf("shared mem created. ID: %i \n",shm_id);
    return shm_id;
}

void resetsignalbuffer(uint64_t * buf, size_t datacount)
{
    for(int i=0;i<datacount;i++)
    {
        buf[i]=0;
    }
    cout<<"SIGNALBUFFER SET to 0"<<endl;
}

int main()
{

    size_t datacount =67108864;
    size_t datasize = datacount *sizeof(uint64_t);
    size_t chunks = 2;
    size_t chunkdatacount = datacount /chunks;
    size_t chunksize = datasize /chunks;
    size_t signalarraysize = chunks * sizeof(uint64_t);
    WallClockStopWatch sw;
    double dsel, dcmp;
    int shm_id_data=0;
    int shm_id_result=0;
    int shm_id_insig=0;
    int shm_id_cmpdone=0;
    key_t key_data = 9988;
    key_t key_result = 9987;
    key_t key_insig = 9985;
    key_t key_cmpdone = 9986;


    //reserving all shared mem buffers
    shm_id_data=reservershm(datasize,key_data);
    shm_id_result=reservershm(datasize,key_result);
    shm_id_insig=reservershm(signalarraysize,key_insig);
    shm_id_cmpdone=reservershm(signalarraysize,key_cmpdone);

    uint64_t * data = (uint64_t *)  shmat(shm_id_data,(void*)0,0);
    uint64_t * res = (uint64_t *)  shmat(shm_id_result,(void*)0,0);
    uint64_t * insig = (uint64_t *)  shmat(shm_id_insig,(void*)0,0);
    uint64_t * cmpdone = (uint64_t *)  shmat(shm_id_cmpdone,(void*)0,0);
    uint64_t * startpointer= res;

    //restting signalling buffers
    resetsignalbuffer(insig, chunks );
    resetsignalbuffer(cmpdone, chunks );

    cout<<"SETTING UP SHARED MEM COMPLETED"<<endl;




    randomnumgen(data, datacount);
    cout<<"RANDOM NUMBERS GENERATED  "<<endl;
    cout<<"STARTING SELECTION "<<endl;

    cout<<"DO SELECTION NOW?  "<<endl;
    std::cin.get();

    sw.start();
    for (int k =0;k<chunks;k++)
    {

        select(data, res, chunkdatacount);
        insig[k]=1; //signals new data for P2 to invoke vhcall

        data+= chunkdatacount;
        res += chunkdatacount;


    }
    sw.stop();
    dsel = sw.duration();
    sw.reset();
    sw.start();



    while(true)
    {
        //waiting until all compress is done
        if(cmpdone[chunks-1]!=0) break;
        nanosleep((const struct timespec[]){{0, 500L}}, NULL);
    }
    cout<<"done waiting for other process...\n ";
    sw.stop();
    dcmp=sw.duration();
    sw.reset();



    cout<<"datasize "<<datacount* sizeof(uint64_t)/(1024*1024)<<" MB"<<" chunks "<<chunks<<endl;
    cout<<"TIME FOR SELECTION: "<<dsel<<endl;
    cout<<"TIME FOR COMPRESS: "<<dcmp<<endl;
    cout<<"TOTAL RUNTIME: "<<dsel+dcmp<<endl;
    //printcmp(startpointer);

//deattach from shared mem
    shmdt(data);
    shmdt(res);
    shmdt(insig);
    shmdt(cmpdone);
//destroy shared mem
    shmctl(shm_id_data,IPC_RMID,NULL);
    shmctl(shm_id_result,IPC_RMID,NULL);
    shmctl(shm_id_insig,IPC_RMID,NULL);
    shmctl(shm_id_cmpdone,IPC_RMID,NULL);

}
