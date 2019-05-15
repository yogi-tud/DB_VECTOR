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

using namespace std;
int main() {

    //define input and result for compression
    __m256i a,result;

    //TODO REPLACE WITH VH DATA STREAM
    //set dummy input data,
    a= _mm256_setr_epi32(3,5,2,7,11,14,15,16);
    printf("input\n");

    for(int i =0;i<8;i++){
        int * in = (int*) &a;

        cout<<in[i]<<" ";
    }
    cout<<endl;
    cout<<"mask:"<<endl;
   /*
    * set bitmask for compress operation
    * 1-element is saved
    * 0-element is discarded
    *
    * example: 0x1A, indices 1,3,4 saved. 2^1 + 2^3 +2^4 = 2+8+16 = 26 = 0x1A (hex)
    */
    __mmask8 mask = 0x1A;

    cout<< bitset<8>(0x1A)<<endl;

    result= _mm256_maskz_compress_epi32(mask, a);
cout<<"output"<<endl;
    for(int i =0;i<8;i++){
        int * in = (int*) &result;

        cout<<in[i] <<" ";

    }
cout<<"WORKS";

    return 0;
}


