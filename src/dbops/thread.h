//
// Created by johannes on 17.05.19.
//


#include <thread>
#include <chrono>
#include <iostream>

//assume isize == osize == 256 * sizeof( uint64_t )

void compress_store( void * hdl, void * ip, void * op ) {
    std::cout << "I start my work immediatly.\n";
    /*__m512i * in = reinterpret_cast< __m512i * >( ip );
    __m512i const set1 = _mm512_set1_epi64( 1 );
    */
    for( int i = 0; i < 10; ++i ) {
        std::cout << i << "\n";
    }
    std::cout << "No I will sleep for 10s\n";
    std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
    std::cout << "I am awake. Lets finish da job.\n";
    for( int i = 10; i < 20; ++i ) {
        std::cout << i << "\n";
    }
    std::cout << "Goodbye.\n";
}

static void * test( void * name ) {
    std::cout << "I start my work immediatly.\n";
    /*__m512i * in = reinterpret_cast< __m512i * >( ip );
    __m512i const set1 = _mm512_set1_epi64( 1 );
    */
    for( int i = 0; i < 10; ++i ) {
        std::cout << i << "\n";
    }
    std::cout << "No I will sleep for 10s\n";
    std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
    std::cout << "I am awake. Lets finish da job.\n";
    for( int i = 10; i < 20; ++i ) {
        std::cout << i << "\n";
    }
    std::cout << "Goodbye.\n";
    return NULL;
}