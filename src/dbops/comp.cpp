//
// Created by johannes on 14.05.19.
//
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <immintrin.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "comp.h"
#include "thread.h"
#include <string.h>

using namespace std;
int main() {


    //define dummy input date
    //TODO REPLACE WITH VHCALL DATA INPUT
    uint64_t input2[]  = {3,5,2,7,11,14,15,16,2,4,1,3,4,4,4,1};
    uint64_t* input=input2;
    uint8_t masks[] = {26,255};
    uint64_t* result= new uint64_t[16];
    uint64_t* startpointer = result;

    int count= compressAll(input,result, masks, 2);

    //compressV(input,result,masks[0],1);

    cout<< "result: "<<endl;
    for(int i=0; i<count;i++){
    cout<<startpointer[i]<<" ";

    }
cout<<endl;

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
 *
 * @param input a pointer to an uint64 array that contains all input data
 * @param masking this int serves as mask to flag input values for compression if it is 1, the value will be written into result
 * Every bit of the mask corrosponds to a position of the input array, ranging from 0 to 7. each int8 is used for 1 run
 * @param runs number of runs perfomed with AVX
 * @return
 */
int compressAll(uint64_t *input, uint64_t *& out, uint8_t * masking, int size)
{
    int count=0;
   for(int i=0;i<size;i++)
   {

    count += compressV(input, out, masking[i],1);
    input += 8;

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
        uint64_t *input, uint64_t *& out, uint8_t masking, int size)
{


    __mmask8 mask = masking;
cout<< "mask: "<<mask;

    //define input AVX register and result AVX register
    __m512i t2;

    //copy input data into AVX data object with offset
    t2= _mm512_load_epi64(input);

    _mm512_mask_compressstoreu_epi64( out, mask, t2 );

    int count = __builtin_popcount(mask);
    //write back the compress data into the result vector

    cout<<"popcount: "<<count<<endl;
    cout<<"compress results: "<<endl;
    for(int i =0;i<count;i++){
        uint64_t * tmp = (uint64_t*) &out;
    cout<< out[i]<<" "<<endl;
    //    resultv.push_back(in[i]);

           }
    cout<< "OUT OLD: "<<out<<endl;

    out += __builtin_popcount(mask);
    cout<< "OUT NEW: "<<out<<endl;

return count;
}