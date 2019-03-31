/** 
*   @file sh_mem.cpp
*   @brief Testing auroras shared memory. creates one shared memory and fill it with "hello world"
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
using namespace std; 

int main()
{


    
int shm_id;       
int err; 
key_t key = 9988;

/*
create and use ftok key, does not work. shmget returns an error in this case
char *path = "/tmp";
int id = 'S';
key_t key = ftok(path,id);
*/

//create shared memory. 
shm_id = shmget(key, 32*sizeof(int), IPC_CREAT|0666);

//shm_id is -1 in case shmget fails to allocate shared memory
if (shm_id < 0) {
     printf("shmget error: %i\n %s ",err,strerror(errno));
     
}
printf("shared mem created. ID: %i \n",shm_id);

 // attach shared memory to proccess memory with shmat. also write string into it
    char *str = (char*) shmat(shm_id,(void*)0,0);
    
    memcpy(str,"Hello World",11);

    printf("%s \n",str);
     
//wait until user input
printf("waiting for other process...\n ");
std::cin.get();
printf("ending process now. freeing ressources \n");

//deattach from shared mem
shmdt(str); 
//destroy shared mem
shmctl(shm_id,IPC_RMID,NULL); 

}
