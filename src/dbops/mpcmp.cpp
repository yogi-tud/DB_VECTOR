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
    size_t datacount = 256000000;
    uint8_t * masks = (uint8_t *) aligned_alloc(8, (datacount ) * sizeof(uint8_t));
    uint64_t * offsets = (uint64_t *) aligned_alloc(8, (datacount / 8) * sizeof(uint64_t));

    uint64_t *data = (uint64_t *) aligned_alloc(64, datacount * sizeof(uint64_t));
    uint64_t *result = (uint64_t *) aligned_alloc(64, datacount * sizeof(uint64_t));
    uint64_t * cresult;
    int n=datacount;
    int n_per_thread;                   // elements per thread
    int total_threads = 48;    // number of threads to use

    cresult = (uint64_t *) malloc(sizeof(uint64_t) * n);
    int i =0;
    n_per_thread = n/(total_threads);



    int backset =0;
    sw.stop();
    double d_init = sw.duration();
    sw.reset();
    sw.start();
    //generate random data

    for (uint64_t i = 0; i < datacount; ++i) {
        data[i] = rand() % 10000;

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



    //calc masks and remember offsets
    for (int i = 0; i < datacount/8 ; i++) {
        masks[i] = createMask(result+8*i);
        //masks[i]=15;

        offsets[i]= _mm_popcnt_u32( masks[i]);

    }
    sw.stop();
    double dmask=sw.duration();
//TODO Umgang mit lücken über threads verteilt. keine lösung möglich? danach memcpys?


  sw.reset();



    sw.start();
    // determine how many elements each process will work on

    //variante 1 COMP auf feste datenblöcke in shared mem. danach mit popcount aufeinanderkopieren
//jeder thread hat einen definierten bereich beim rückschreiben
//privaten pointer = threadid * chunksize

   // omp_set_dynamic(0);     // Explicitly disable dynamic teams
   // omp_set_num_threads(total_threads);
    /*
#pragma omp parallel for shared(cresult, data, masks, offsets) private(i) schedule(dynamic, n_per_thread)
    for(i=0; i<n/8; i++) {



        _mm512_mask_compressstoreu_epi64(cresult + i * offsets[i], masks[i], _mm512_loadu_si512(data + i * 8) );

    }*/

    for(i=0; i<datacount/8; i++) {



        _mm512_mask_compressstoreu_epi64( cresult+i*offsets[i],  masks[i], _mm512_loadu_si512(data+i*8) );

    }

    sw.stop();
    double dcmp = sw.duration();
    cout<<"datacount ; threads "<<datacount<<" "<<total_threads<<endl;
    cout<<"[VH] duration compress "<<dcmp<<endl;
    cout<<"[VH] duration mask "<<dmask<<endl;
    cout<<"[VH] duration init "<<d_init<<endl;
/*
    cout<<"[VH] compress result:"<<endl;
    for(int h =0; h < datacount; h++)
    {
        cout<<(uint64_t)cresult[h]<<" "<<data[h]<<endl;
    }
    */
    cout<<"[VH] Lücken test:"<<endl;
    int fail=0;
    for(int h =0; h < datacount/8; h++)
    {

        if((int)masks[h]==0){

        fail=1;
    }}
    if(fail==1)
    {
        cout<<"[VH] Compress failed, lücken in result"<<endl;
    }
    else
    {
        cout<<"[VH] Compress successfull"<<endl;
    }




    return 0;

}