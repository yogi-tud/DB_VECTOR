#include <iostream>
#include "stopwatch.h"
#include "memory.h"
#include <omp.h>
#include <immintrin.h>

using namespace std;

inline __attribute__((always_inline)) __mmask8 createMask(
        uint64_t * maskin ) {



    return _mm512_cmpeq_epi64_mask( _mm512_load_epi64( maskin ),
                                    _mm512_set1_epi64( 1 ) );



}



int main() {


    std::cout << "[VH] Start" << std::endl;

    WallClockStopWatch sw;
    sw.start();
    uint32_t lower = 5000;
    uint32_t upper = 10000;
    size_t datacount = 1024000000;
    uint8_t * masks = (uint8_t *) aligned_alloc(8, (datacount ) * sizeof(uint8_t));


    uint64_t *data = (uint64_t *) aligned_alloc(64, datacount * sizeof(uint64_t));
    uint64_t *result = (uint64_t *) aligned_alloc(64, datacount * sizeof(uint64_t));
    uint64_t * cresult;
    int n=datacount;
    int n_per_thread;                   // elements per thread
    int total_threads = 48;    // number of threads to use

    cresult = (uint64_t *) malloc(sizeof(uint64_t) * n);
    int i =0;
    n_per_thread = n/(total_threads);

    for(int i =0;i<datacount;i++)
    {
        cresult[i]=0;
    }

    int backset =0;
    sw.stop();
    double d_init = sw.duration();
    sw.reset();
    sw.start();
    //generate random data

    for (uint64_t i = 0; i < datacount; ++i) {
        data[i] = rand() % 10000;
       // data[i]=i;
    }

    sw.stop();
    double d_rand = sw.duration();

    sw.reset();


//Selection calculation

//set up constants



    sw.start();

    for(uint64_t i = 0; i < datacount; ++i ) {
        result[ i] = ( data[ i ] < upper ) & ( data[ i ] > lower ) ;
    }



    int hits=0;


    for(int f=0; f < datacount; f++)
    {
        if(result[f]){
            hits++;
        }

    }
    float phit = (float)hits / (float)datacount;
    sw.stop();
    double d_sel = sw.duration();
    sw.reset();
    cout<<"[VH] Selection done"<<endl;
    sw.start();



    uint64_t * offsets = (uint64_t *) aligned_alloc(8, ((datacount / 8)+1) * sizeof(uint64_t));
    //calc masks and remember offsets
#pragma omp parallel for shared(result, masks) private(i) schedule(dynamic, n_per_thread)
    for (int i = 0; i < datacount/8 ; i++) {
        masks[i] = createMask(result+8*i);

        //masks[i]=240;


    }
    //masks[31]=0;
    sw.stop();
    double dmask=sw.duration();
    sw.reset();
    sw.start();
    int sum=0;



    for(int i =0;i<(datacount/8);i++)
    {
        offsets[i]=sum;

        sum+=_mm_popcnt_u32( masks[i]);


    }
    offsets[0]=0;

    sw.stop();
    double doff=sw.duration();
    //Offsets holds the number of steps the result pointer must be incremented before writing back the compress result
    //example: mask: 11110000,11110000,11110000,11110000
    //offsets 0-2: 0,4,8,12
    //the first element is 0, begin result writing at the start of result buffer


  sw.reset();



    sw.start();
    // determine how many elements each process will work on

    //variante 1 COMP auf feste datenblöcke in shared mem. danach mit popcount aufeinanderkopieren
//jeder thread hat einen definierten bereich beim rückschreiben
//privaten pointer = threadid * chunksize

    //omp_set_dynamic(0);     // Explicitly disable dynamic teams
   // omp_set_num_threads(total_threads);
   //TEST CPU LOAD, REMOVE LOOP AFTERWARDS
   for(int k=0;k<500;k++) {

#pragma omp parallel for shared(cresult, data, masks, offsets) private(i) schedule(dynamic, n_per_thread)
       for (i = 0; i < n / 8; i++) {


           _mm512_mask_compressstoreu_epi64(cresult + offsets[i], masks[i], _mm512_loadu_si512(data + i * 8));

       }
   }

    sw.stop();
    double dcmp = sw.duration();
    cout<<"datacount ; threads "<<datacount<<" "<<total_threads<<endl;
    cout<<"[VH] duration compress "<<dcmp<<endl;
    cout<<"[VH] duration mask calc "<<dmask<<endl;
    cout<<"[VH] duration offset calc "<<doff<<endl;
    cout<<"[VH] duration init "<<d_init<<endl;
/*
cout<<"[VH] offsets: "<<endl;
for(int k=0;k<datacount/8;k++)
{
    cout<<offsets[k]<<" ";
}

    cout<<"\n[VH] cmp | input | index:"<<endl;
    for(int h =0; h < datacount; h++)
    {
        cout<<(uint64_t)cresult[h]<<" "<<data[h]<<" " <<h<<endl;
    }
*/





    return 0;

}