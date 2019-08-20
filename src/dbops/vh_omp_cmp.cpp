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
    static __mmask8 createMask(uint64_t  * maskin);



};
inline __attribute__((always_inline)) __mmask8 Parent::createMask(
        uint64_t * maskin ) {



    return _mm512_cmpeq_epi64_mask( _mm512_loadu_si512( maskin ),
                                    _mm512_set1_epi64( 1 ) );



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
 *
 */

inline __attribute__((always_inline)) int Parent::compressV(uint64_t *input, uint64_t *&out, uint8_t masking) {


    //uint64_t * ain = (uint64_t *) aligned_alloc(64,8*sizeof(uint64_t));
    //memcpy(ain,input,64);

    _mm512_mask_compressstoreu_epi64( out, (__mmask8) masking, _mm512_loadu_si512(input) );
    int count = _mm_popcnt_u32( masking); //  oder

    out += count;

    //  free(ain);

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

    for (int i = 0; i < size; i++) {
        if (masking[i] == 0 ) {
            input += 8;
            continue;
        }


        count += Parent::compressV(input, out, masking[i]);

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



    WallClockStopWatch sw;
    sw.start();

    uint8_t * masks = (uint8_t *) aligned_alloc(8, datasize / 8);
    uint64_t * offsets = (uint64_t *) aligned_alloc(8, ((datasize / 8)+1) * sizeof(uint64_t));

    int n=datasize;
    int n_per_thread;                   // elements per thread
    int total_threads = 48;    // number of threads to use


    int i =0;
    n_per_thread = n/(total_threads);
    sw.stop();
    double dinit =sw.duration();
    sw.reset();


    sw.start();

    //calc masks openmp
#pragma omp parallel for shared(res, masks) private(i) schedule(dynamic, n_per_thread)
    for (int i = 0; i < datasize/8 ; i++) {
        masks[i] = Parent::createMask(res+8*i);


    }

    sw.stop();
    double dmask=sw.duration();
    sw.reset();
    sw.start();
    int sum=0;

    //calculating OFFSET array for openmp compress
    for(int i =0;i<(datasize/8);i++)
    {
        offsets[i]=sum;

        sum+=_mm_popcnt_u32( masks[i]);


    }
    offsets[0]=0;
    sw.stop();
    double doff=sw.duration();
    sw.reset();
    sw.start();
    //openmp compress
#pragma omp parallel for shared(res, in, masks, offsets) private(i) schedule(dynamic, n_per_thread)
    for (i = 0; i < n / 8; i++) {


        _mm512_mask_compressstoreu_epi64(res + offsets[i], masks[i], _mm512_loadu_si512(in + i * 8));


    }



    sw.stop();

   double dcmp = sw.duration();
  double total = dcmp+dmask+dinit+doff;

  /*
   cout<<"[VH] datacount ; threads \n"<<datasize<<" "<<total_threads<<endl;
   cout<<"[VH] datasize "<<datasize* sizeof(uint64_t)/(1024*1024)<<" MB"<<endl;
   cout<<"[VH] duration compress "<<dcmp<<endl;
   cout<<"[VH] duration mask calc "<<dmask<<endl;
   cout<<"[VH] duration offset calc "<<doff<<endl;
   cout<<"[VH] duration init "<<dinit<<endl;
   cout<<"[VH] duration total: "<<total<<endl;
*/


    return  offsets[(datasize/8)-1];  //return number of compressed values in result buffer


}

}