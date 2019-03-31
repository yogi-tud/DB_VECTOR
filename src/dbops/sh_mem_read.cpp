/** 
*   @file sh_mem.cpp
*   @brief Testing auroras shared memory. reads from shared memory, created by another process
*
*
*   @author Johannes Fett
*
*   @date 2/4/2019
*/


#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <stdio.h>
#include<iostream>


int main()
{


int shm_id=0;       
int err=0; 


shm_id = shmget(9988, 4*sizeof(int), err);

if (shm_id < 0) {
    printf("shmget error: %i\n %s ",err,strerror(errno));
    
}

 // shmat to attach to shared memory 
    char *str = (char*) shmat(shm_id,(void*)0,0); 
  
    printf("Data read from memory: %s\n",str); 

     //detach from shared memory  
    shmdt(str); 

}
