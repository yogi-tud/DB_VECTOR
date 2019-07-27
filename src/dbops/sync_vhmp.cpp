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



    return _mm512_cmpeq_epi64_mask( _mm512_load_epi64( maskin ),
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


//#pragma omp parallel num_threads(1)
    //{
    //
//#pragma  omp for ordered schedule(dynamic)
    for (int i = 0; i < size; i++) {
        if (masking[i] == 0 ) {
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







    uint64_t *result = (uint64_t *) aligned_alloc(8, datasize * sizeof(uint64_t));   //buffer of compress results
    uint64_t *startpointer = result;
    int count=0;

    uint8_t * masks = (uint8_t *) aligned_alloc(8, datasize / 8);

    for (int i = 0; i < datasize / 8; i++) {
        masks[i] = Parent::createMask(&res[i * 8]);


    }
    //cerr << "[VH] MASKS GENERATED" << endl;



    count = Parent::compressAll(in, result, masks, datasize / 8);  //Do compress on all input



    //cout<< "[VH] result: "<<endl;
    // for(int i=0; i<count;i++){
    //    cout<<startpointer[i]<<" ";

    // }
    //cout << endl;

    memcpy(res, startpointer, datasize * sizeof(uint64_t)); //copy back result to result input buffer

    // free(result);
    // cout<<"[VH] MEMCPY COMPRESS RESULTS DONE"<<endl;


    delete (startpointer);




    return (uint64_t) count;  //return number of compressed values in result buffer


}

}

int main () {
    WallClockStopWatch sw;
    sw.start();
    uint32_t lower = 5000;
    uint32_t upper = 10000;
    size_t datasize = 256;

    uint64_t *data = (uint64_t *) aligned_alloc(64, datasize * sizeof(uint64_t));
    uint64_t *result = (uint64_t *) aligned_alloc(64, datasize * sizeof(uint64_t));

    //generate random data

    for (uint64_t i = 0; i < datasize; ++i) {
        data[i] = rand() % 10000;

    }

    sw.stop();
    double d_rand = sw.duration();

    sw.reset();


//Selection calculation

//set up constants



    sw.start();

    for( uint64_t i = 0; i < datasize; ++i ) {
        result[ i] = ( data[ i ] < upper ) & ( data[ i ] > lower ) ;
    }



    int hits=0;


    for(int f=0;f<datasize;f++)
    {
        if(result[f]){
            hits++;
        }

    }
    float phit = (float)hits / (float)datasize;
    sw.stop();
    double d_sel = sw.duration();

    cout<<"DONE"<<endl;



    return 0;
}