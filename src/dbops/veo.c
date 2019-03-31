#include <ve_offload.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std;
using namespace chrono;

int main()
{
    int datasize = 2000000000;
    

    std::vector<uint32_t> data(datasize);
    std::vector<uint32_t> result(datasize);
    auto start= high_resolution_clock::now();

    //iterator auf 1. element (data.begin())
    
    std::transform(data.begin(),data.end(),data.begin(),
    [](const uint32_t& in)
    {
        return rand() % 10000;
    });

  

    
    auto randomIntGenerationTime= high_resolution_clock::now()-start;
    cout<<"time to create random ints: "<<duration_cast<milliseconds>(randomIntGenerationTime).count()<<" ms"<<endl;


  /* Load "veo_lib" on VE node 0 */
  struct veo_proc_handle *proc = veo_proc_create_static(0, "./veo_lib");
  uint64_t handle = NULL;/* find a function in the executable */
  struct veo_thr_ctxt *ctx = veo_context_open(proc);
  struct veo_args *argp = veo_args_alloc();

//status code int
  int rc=-5; 

//allocate memory in ve proc space
//will be used to hold input of random INTS
uint64_t ve_addr_int;
uint64_t ve_addr_bool;
//set datasize for memory on VE, numbers of int


//allocate int array on VE. used for input data
rc = veo_alloc_mem(proc, &ve_addr_int, datasize*sizeof(uint32_t));
printf("alloc input array: errorcode: %i\n",rc);
//allocate bool array on ve. used for result data
rc = veo_alloc_mem(proc, &ve_addr_bool, datasize*sizeof(uint32_t));
printf("alloc res array: errorcode: %i\n",rc);
//transfer input data to VE
rc = veo_write_mem(proc, ve_addr_int, data.data(),  datasize*sizeof(uint32_t));

/*
* function arguments must be set both in VH and VE code
* setting up necessary argtuments, which are
*   Nr    Argument
*   0     datasize int, 
*   1     data int*, 
*   2     results int*,
*   
*
*   TODO: all selection parameters must be passed as arguments
*
*   datasize: number of input ints for selection
*   data: a pointer to the input ints on VE memory
*   results: a pointer to result bool array on VE memory
*   hits: an integer that counts the selection hits
*/
/* allocate VEO function call arguments struct */

printf("VH side: int pointer: %i \n",ve_addr_int);
printf("VH side: int res pointer: %i \n",ve_addr_bool);

rc = veo_args_set_u64(argp, 0, datasize);
//a pointer to the data array must be passed as uint as function argument. i hope
rc = veo_args_set_u64(argp, 1, ve_addr_int);
//same as last one. as bool is not supported, the result array is int as well.
rc = veo_args_set_u64(argp, 2, ve_addr_bool);



  uint64_t id = veo_call_async_by_name(ctx, handle, "hello", argp);
  uint64_t retval;
  veo_call_wait_result(ctx, id, &retval);
 /*
  auto selT= start;
printf("time to do selection: %i ms\n",selT);
*/
//transfer result array back to VH
rc = veo_read_mem(proc, result.data(), ve_addr_bool, datasize*sizeof(uint32_t));
//auto cpT= start;
//printf("time to copy result from VE to VH: %i ms\n",cpT);
//printf("MEM READ ERROR CODE: %i \n",rc);

  float phit = (float)retval / (float)datasize;
  printf("hits: %i  percentage of hits: %f \n",retval,100*phit);
  veo_args_clear(argp);
  veo_args_free(argp);
  veo_context_close(ctx);

  
  return 0;
}
