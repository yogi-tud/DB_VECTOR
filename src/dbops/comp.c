//
// Created by johannes on 14.05.19.
//


#include <stdint.h>
#include <immintrin.h>
#include "comp_c.h"
#include <stdio.h>
#include <stdlib.h>



int main() {


    //define dummy input date
    //TODO REPLACE WITH VHCALL DATA INPUT
    uint64_t* input = aligned_alloc(256,256*sizeof(uint64_t *));
    uint64_t* result= aligned_alloc(256,256*sizeof(uint64_t *));
    printf("\nINPUT: \n");
    for(int i=0;i<256;i++)
    {
        input[i]=i;
        printf(" %i ",input[i]);
    }

    uint8_t masks[3] = {26,16,255};

    uint64_t* startpointer = result;

    int count= compressAll(input,result, masks, 3);


    printf("count total: %i \n",count);
    printf("total results: \n");
    for(int i=0; i<count;i++){

    printf(" %i ",startpointer[i]);
    }

    //TEST OF MASK GENERATION FROM UINT64 DUMMY DATA ARRAYS

    uint64_t* maskin = aligned_alloc(64,8*sizeof(uint64_t *));
    for(int i=0;i<8;i++)
    {
        maskin[i]=0;
    }
    maskin[0]=1;
    maskin[3]=1;

    __mmask8 masktest = createMask(maskin);

    printf("\n MASK EQUI GENERATION TEST: %i \n",(uint8_t) masktest);

    //compressV(in,26,0);

/*
 *
    void * a = malloc( 10 );
    void * b = malloc( 20 );

    std::cout << "Hello from main. Starting da thread.\n";

    std::thread t( compress_store, a,b,b );

    t.detach();
*/
    /*pthread_t t1;
    int ret = pthread_create( &t1, NULL, test, NULL );
    if( ret == 0 ) {
       std::cout << "Thread created. Detaching...\n";
 //      pthread_detach( t1 );
    }*/

/*
    free( b );
    free( a );
    std::cout << "Goodby from main\n";



    pthread_exit(0);
    */
    return 0;
}
/***
 * @brief This function creates a mask for avx instruction from uint64_t input.
 * If mask[i]=input[i] (ONLY 1,0 INPUT IS ACCEPTABLE)
 * @param maskin input data. only acceptable values are 1 and 0
 * @return a __mask8 that contains a mask for AVX operations
 */
__mmask8 createMask(uint64_t* maskin)
{


    uint64_t* maskstatic= aligned_alloc(64,8*sizeof(uint64_t *));

    for(int i=0;i<8;i++)
    {
        maskstatic[i]=1;

    }

    __m512i m1,m2;


    m1= _mm512_load_epi64(maskin);
    m2= _mm512_load_epi64(maskstatic);


    __mmask8 masktest = _mm512_cmp_epi64_mask (m1,m2, _MM_CMPINT_EQ);


    return masktest;
}


/***
 *
 * @param input a pointer to an uint64 array that contains all input data
 * @param masking this int serves as mask to flag input values for compression if it is 1, the value will be written into result
 * Every bit of the mask corrosponds to a position of the input array, ranging from 0 to 7. each int8 is used for 1 run
 * @param runs number of runs perfomed with AVX
 * @return
 */
int compressAll(uint64_t *input, uint64_t * out, uint8_t * masking, int size)
{
int count=0;
int offset=0;
uint64_t *startpointer = out;
    __m512i t2;

for(int i=0;i<size;i++)
{

//count += compressV(input, out, masking[i]);



    __mmask8 mask = masking[i];

    t2= _mm512_load_epi64(input);

    _mm512_mask_compressstoreu_epi64( out, mask, t2 );

    offset= __builtin_popcount(mask);
    count += offset;

input += 8;
out += offset;

}
printf("\nINSIDE CHECK: \n");
    for(int i=0; i<count;i++){

        printf(" %i ",startpointer[i]);
    }


return count;


}



/***
 * @brief This function performs AVX function _mm512_maskz_compress_epi64 on 8 64 bit input values and returns the result
 * Example: input 3,5,2,7,11,14,15,16
 * mask: 01011000
 * resulting vector: 5,7,11
 * @param input a vector of input integers that will be compressed via AVX.
 * If the vector is larger than 8 entries, only the first 8 will be used
 * @param masking this int serves as mask to flag input values for compression if it is 1, the value will be written into result
 * Every bit of the mask corrosponds to a position of the input array, ranging from 0 to 7.
 * @param offset offset to be applied on input data. shifts the access offset steps to the right.
 * @return resultsize
 */

int compressV(
        uint64_t *input, uint64_t * out, uint8_t masking)
{


__mmask8 mask = masking;

//define input AVX register and result AVX register
__m512i t2;

//copy input data into AVX data object with offset
 t2= _mm512_load_epi64(input);

 //DO compress and save it to result buffer
_mm512_mask_compressstoreu_epi64( out, mask, t2 );

int count = __builtin_popcount(mask);


for(int i =0;i< count;i++)
{
    printf(" %i ", out[i]);
}
printf("\n");
//return offset for the resultbuffer
return count;
}