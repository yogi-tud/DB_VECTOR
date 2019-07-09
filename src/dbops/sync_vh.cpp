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


using namespace std;
class Parent
{
public:
   void showMessage(const char* message);
   static int compressV(uint64_t *input, uint64_t *&out, uint8_t masking, int size);
   static int compressAll(uint64_t *input, uint64_t *&out, uint8_t * masking, int size);
   static __mmask8 createMask(uint64_t* maskin);

};

__mmask8 Parent::createMask(uint64_t* maskin)
{


    uint64_t* maskstatic= (uint64_t *) aligned_alloc(64,8*sizeof(uint64_t *));

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

void Parent::showMessage(const char* message)
{
    std::cout << "[message]" << message << std::endl;
}

int Parent::compressV(uint64_t *input, uint64_t *&out, uint8_t masking, int size)
{

    __mmask8 mask = masking;
   // cout<< "mask: "<<mask;

    //define input AVX register and result AVX register
    __m512i t2;

    //copy input data into AVX data object with offset
    t2= _mm512_load_epi64(input);

    _mm512_mask_compressstoreu_epi64( out, mask, t2 );

    int count = __builtin_popcount(mask);
    //write back the compress data into the result vector

   // cout<<"popcount: "<<count<<endl;
  //  cout<<"compress results: "<<endl;
    for(int i =0;i<count;i++){
        uint64_t * tmp = (uint64_t*) &out;
      //  cout<< out[i]<<" "<<endl;
        //    resultv.push_back(in[i]);

    }
   // cout<< "OUT OLD: "<<out<<endl;

    out += __builtin_popcount(mask);
   // cout<< "OUT NEW: "<<out<<endl;

    return count;

}

int Parent::compressAll(uint64_t *input, uint64_t *&out, uint8_t* masking, int size)
{
    int count=0;
    for(int i=0;i<size;i++)
    {
        if(__builtin_popcount(masking[i])==0)
        {   input+=8;
            continue;
        }

        count += Parent::compressV(input, out, masking[i],1);
        input += 8;

    }

    return count;

}

extern "C" {

uint64_t compress(uint64_t * in, uint64_t * res,  veos_handle *handle)
{
    Parent p;

    cout<<res<<endl;
    cout<<in<<endl;
    cout<<endl<<"test in:";
        //print INPUT from VE
        for(int i =0;i<256;i++)
        {
            cout<<in[i]<<" ";
        }
        cout<<endl<<"test hit:";

    for(int i =0;i<256;i++)
    {
        cout<<res[i]<<" ";
    }



    cout<<endl;

    //input buffer of selection hits must be copied to properly aligned memory, due to AVX being picky about alignment
    uint64_t* selhits = (uint64_t *) aligned_alloc(256,256*sizeof(uint64_t *));
    memcpy( selhits, res, 256*sizeof(uint64_t));
    uint64_t* result= (uint64_t *) aligned_alloc(256,256*sizeof(uint64_t *));


//INPUT: result array [256] conatinian 1/0 to indicate hit/miss of selection
//INPUT: numbers [256] array of slection input data, which must be compressed



    uint8_t masks[32]= {0};
    //calculate masks from input (selection hits)
    for(int i=0;i<32;i++)
    {
        masks[i]=Parent::createMask(&selhits[i*8]);
        cout<<"mask: "<<(unsigned int) masks[i]<<endl;
    }


    uint64_t* startpointer = result;

    //Do compress on all input
    int count= Parent::compressAll(in,result, masks, 32);

    //compressV(input,result,masks[0],1);

    cout<< "result: "<<endl;
    for(int i=0; i<count;i++){
        cout<<startpointer[i]<<" ";

    }
    cout<<endl;

    //SET input pointer to result array!
    in = startpointer;

    return 0;
}

}