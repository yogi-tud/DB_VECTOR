/** 
*   @file vhcode.c
*   @brief This file will perform a selection operator with random generated int numbers  on a NEC Aurora Vector engine 
*   
*   
*
*	This file will be executed as library on the vector host.
*	
*	For all elements in the int array, random numbers are generated.
*	hdl: process handle for veos. used to communicate between VE and VH
*	void *ip : pointer on input element. Which is the struct sr from vhcall_demo.c
*	size_t isize: size of the struct
*	void *op: can be used to specify an output buffer
*	size_t osize: size of output buffer
*   @author Johannes Fett
*
*   @date 3/24/2019
*/


#include "libvepseudo.h"
#include "vh_lib.h"
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>






int64_t
generateInts(void *hdl, void *ip, size_t isize, void *op, size_t osize)
{
	 
	int ret;
	//our output buffer of ints
	void *vh_buffer = NULL;
	//struct from VE side
	struct sendrecv_data *data = ip;

	if (isize != sizeof(struct sendrecv_data)) {
		return 1;
	}
	vh_buffer = malloc(data->size);
	
	if (!vh_buffer) {
		perror("vh_buffer malloc");
		return 1;
	}
	ret = ve_recv_data(hdl, (uint64_t)data->addr, data->size, vh_buffer);
	if (ret != 0) {
		return 1;
	}
	printf("received buffer on VH. size: %i\n", isize/4);
	uint32_t * output = (uint32_t*) vh_buffer;
	int limit = data->size/4;

	//printf("TESTING DATA HANDOVER, DATA OF 1. element: %i \n",data->idata[0]);

	
	printf("DATA ELEMENTS AT VE: %i \n",limit);
	
	for (int i = 0; i < limit; i++) {
	  
	  
	  output[ i ]= rand() % 10000;
		//printf("i: %i wert: %i \n",i,output[ i ]);
	}


	
	printf("Start data transfer to VE \n");
	ret = ve_send_data(hdl, (uint64_t)data->result, data->size, vh_buffer);
	
	printf("sent buffer to VE:   \n");
	free(vh_buffer);
	return 0;
}

int64_t
gen_num(void *hdl, void *ip, size_t isize, void *op, size_t osize)
{
	//isize is the input size in BYTES. so isize/4 is the number of ints in the buffer
	printf("size: %i\n",isize);
	int *in = (int *)ip;
	int *out = (int *)op;
	


	
   
	
	printf("VH side done\n");
	return 0;
}
