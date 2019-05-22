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

    vector<uint64_t> input = {3,5,2,7,11,14,15,16,2,4,1,3,4,4,4,1};
    vector<uint8_t> masks = {255,255};
    vector<uint64_t> result = compressAll(input, masks);


    cout<< "result: "<<endl;
    for(int i=0; i<result.size();i++){
    cout<<result[i]<<" ";

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
 * @param input a vector of input integers that will be compressed via AVX. Constraint: input.size() = runs *8
 * @param masking this int serves as mask to flag input values for compression if it is 1, the value will be written into result
 * Every bit of the mask corrosponds to a position of the input array, ranging from 0 to 7. each int8 is used for 1 run
 * @param runs number of runs perfomed with AVX
 * @return
 */
vector<uint64_t> compressAll(vector<uint64_t> input, vector<uint8_t> masking)
{
    //check if input data and number of runs fit. condition: input.size() = runs *8
    if(input.size()!=masking.size()*8)
    {
        cout<<"DATASIZE INSUFFICIENT! STOPPING"<<endl;
        cout<<"DATASIZE NEEDED: "<<masking.size()*8<<endl;
        cout<<"DATASIZE CURRENT: "<<input.size()<<endl;
        exit(1);
    }

vector<uint64_t> compressed,tmpv;
    //each run performs one AVX compress von 8 64 bit values
for(int i=0;i<masking.size();i++){
    tmpv=compressV(input, masking[i],i*8);
compressed.insert(compressed.end(), tmpv.begin(),tmpv.end());
}
return compressed;
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
 * @return the compressed vector.
 */

vector<uint64_t> compressV(vector<uint64_t> input, uint8_t masking, int o)
{
    //result vector, contains compressed values from AVX compress
    vector<uint64_t> resultv;

    bitset<8> mybits(0x1A);
    unsigned char let=0;

    //copy the mask bits from input uint8_t masking to unsigned char let
    //as __mask8 is an unsigned char, this is needed to use the mask for AVX
    memcpy(&let, &masking,sizeof(unsigned char));
    __mmask8 mask = let;

    //define input AVX register and result AVX register
    __m512i t2,result2;

    //copy input data into AVX data object with offset
    t2= _mm512_setr_epi64(input[0+o],input[1+o],input[2+o],input[3+o],input[4+o],input[5+o],input[6+o],input[7+o]);
    //perform AVX compress
    result2= _mm512_maskz_compress_epi64(mask, t2);

    //write back the compress data into the result vector
    for(int i =0;i<8;i++){
        uint64_t * in = (uint64_t*) &result2;

        resultv.push_back(in[i]);

            }


return resultv;
}