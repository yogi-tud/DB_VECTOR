//
// Created by johannes on 28.06.19.
//

#include <iostream>
#include <inttypes.h>
#include <stdio.h>
#include <immintrin.h>
#include <string.h>
#include <unistd.h>
#include <array>
#include "libvepseudo.h"
#include <sched.h>
#include "stopwatch.h"


using namespace std;
class Parent
{
public:
   void showMessage(const char* message);
   static int compressV(uint64_t *input, uint64_t *&out, uint8_t masking);
   static int compressAll(uint64_t *input, uint64_t *&out, uint8_t * masking, int size);
   static __mmask8 createMask(uint64_t* maskin);



};

__mmask8 Parent::createMask(uint64_t* maskin)
{


    alignas(8) uint64_t maskstatic[8];

    for(int i=0;i<8;i++)
    {
        maskstatic[i]=1;

    }

    __m512i m1,m2;

    alignas(8) uint64_t hits[8];
    memcpy( hits, maskin, 8*sizeof(uint64_t));


    m1= _mm512_load_epi64(hits);

    m2= _mm512_load_epi64(maskstatic);


    __mmask8 masktest = _mm512_cmp_epi64_mask (m1,m2, _MM_CMPINT_EQ);


    return masktest;
}

void Parent::showMessage(const char* message)
{
    //cout << "[message]" << message << std::endl;
}

/***
 * @brief this function performs a single AVX compress instruction on input data
 * @param input pointer to 8 input values for compress
 * @param out pointer to write result of compress back to
 * @param masking a mask with 8 bits
 * @return popcount of a single compress run
 */
int  Parent::compressV(uint64_t *input, uint64_t *&out, uint8_t masking)
{

    __mmask8 mask = masking;
   // //cout<< "mask: "<<mask;

    //define input AVX register and result AVX register
    __m512i t2;

    //copy input data into AVX data object with offset
    alignas(8) uint64_t ainput[8];
    memcpy( ainput, input, 8*sizeof(uint64_t));



    t2= _mm512_load_epi64(ainput);


    _mm512_mask_compressstoreu_epi64( out, mask, t2 );


    int count = __builtin_popcount(mask);
    //write back the compress data into the result vector

   // //cout<<"popcount: "<<count<<endl;
  //  //cout<<"compress results: "<<endl;
    for(int i =0;i<count;i++){
        uint64_t * tmp = (uint64_t*) &out;
      //  //cout<< out[i]<<" "<<endl;
        //    resultv.push_back(in[i]);

    }
   // //cout<< "OUT OLD: "<<out<<endl;

    out += __builtin_popcount(mask);
   // //cout<< "OUT NEW: "<<out<<endl;



    return count;

}
/***
 * @brief this function performs size times runs of avx compress and counts the total number of result values
 * @param input in buffer with selection input values. must be divisable by 8
 * @param out address of the output buffer. compress results will be written here
 * @param masking masks for performance avx compress. number of masks must equal size parameter
 * @param size number of compress runs to be performed. each run uses 8 input values and 1 mask
 * @return popcount of all compress runs in total
 */
int  Parent::compressAll(uint64_t *input, uint64_t *&out, uint8_t* masking, int size)
{
    int count = 0;


//#pragma omp parallel num_threads(1)
    //{
   //
//#pragma  omp for ordered schedule(dynamic)
        for (int i = 0; i < size; i++) {
            if (__builtin_popcount(masking[i]) == 0) {
                input += 8;
                continue;
            }
           // int thread_num = omp_get_thread_num();
           // int cpu_num = sched_getcpu();
           // printf("Thread %3d is running on CPU %3d\n", thread_num, cpu_num);
//#pragma omp ordered
            count += Parent::compressV(input, out, masking[i]);
//#pragma omp ordered
            input += 8;

       // }
    }
    return count;

}

extern "C" {
/***
 * @brief performs avx compress on input data on VH. After the calculation, the result is written back to the VE.
 * @param handle veos handle to access veos features
 * @param in buffer with 256 selection input values
 * @param res buffer with 256 selection results {0,1}
 * @return total popcount of compress
 */

uint64_t compress(veos_handle *handle, uint64_t * in, uint64_t * res,  uint64_t datasize)
{
    cerr<<"[VH] DATASIZE: "<<datasize<<endl;

    Parent p;
    WallClockStopWatch sw;
    //int datasize =256;

    cerr<<"[VH] DATASIZE: "<<datasize<<endl;


    //cout<<endl;

    //input buffer of selection hits must be copied to properly aligned memory, due to AVX being picky about alignment
    sw.start();
   // alignas(8) uint64_t selhits[datasize];
  // memcpy( selhits, res, datasize*sizeof(uint64_t));
    //cout<<"[VH] MEMCPY SELECT RESULTS DONE"<<endl;

    uint64_t* result= (uint64_t *) aligned_alloc(8,datasize*sizeof(uint64_t ));   //buffer of compress results
   // cout<< "[VH] RESULT DATA ALLOCATED"<<endl;



   // alignas(8) uint64_t ain[datasize];
   // memcpy( ain, in, datasize*sizeof(uint64_t));
    //cout<<"[VH] MEMCPY INPUT DONE"<<endl;

    cerr<< "[VH] BEFORE MASKS"<<endl;

    uint8_t masks[datasize/8]= {0};
    //calculate masks for avx instructions from input (selection hits)
    for(int i=0;i<datasize/8;i++)
    {
        masks[i]=Parent::createMask(&res[i*8]);
       // cout<<"mask: "<<(unsigned int) masks[i]<<endl;
    }
    cerr<<"[VH] MASKS GENERATED"<<endl;


    uint64_t* startpointer = result;

    int count= Parent::compressAll(in,result, masks, datasize/8);  //Do compress on all input



    //cout<< "[VH] result: "<<endl;
   // for(int i=0; i<count;i++){
   //    cout<<startpointer[i]<<" ";

   // }
    cout<<endl;
    cerr<<"[VH] vormem: "<<datasize<<endl;
    memcpy( res, startpointer, datasize*sizeof(uint64_t)); //copy back result to result input buffer
    cerr<<"[VH] nachmem: "<<datasize<<endl;
    free(result);
   // cout<<"[VH] MEMCPY COMPRESS RESULTS DONE"<<endl;
    sw.stop();
    double d_mcpy = sw.duration();


    return (uint64_t) count;  //return number of compressed values in result buffer


}

}