// author:
#ifndef TUD_MEMORY_H
#define TUD_MEMORY_H

#include <cstdint>
#include <cstddef>

#define NEC_ALIGNMENT_BYTE 128
#define NEC_ALIGNMENT_TWOS_COMPLEMENT -NEC_ALIGNMENT_BYTE
#define NEC_ALIGNMENT_MINUS_ONE_BYTE NEC_ALIGNMENT_BYTE-1
#define NEC_VECSIZE 0x800

#define INTEL_ALIGNMENT_BYTE 64
#define INTEL_ALIGNMENT_TWOS_COMPLEMENT -INTEL_ALIGNMENT_BYTE
#define INTEL_ALIGNMENT_MINUS_ONE_BYTE INTEL_ALIGNMENT_BYTE-1
#define INTEL_VECSIZE 0x40

size_t get_size_with_alignment_padding_nec( size_t p_SizeOfMemChunk ) {
   return p_SizeOfMemChunk+NEC_ALIGNMENT_MINUS_ONE_BYTE;
}

void* create_aligned_ptr_nec(void* p_Ptr) {
   size_t ptrToSizeT = reinterpret_cast<size_t>(p_Ptr);
   size_t offset = ptrToSizeT & NEC_ALIGNMENT_MINUS_ONE_BYTE;
   return reinterpret_cast<void*>( ptrToSizeT + NEC_ALIGNMENT_BYTE + (-offset | NEC_ALIGNMENT_TWOS_COMPLEMENT) );
}


template< typename T >
T * nec_malloc( size_t count ) {
   size_t datasize = get_size_with_alignment_padding_nec( ( count * sizeof( T ) ) + sizeof( T* ) );
   void * malloc_ptr = malloc( datasize );
   void * ptr = static_cast< void * >( static_cast< char * >( malloc_ptr ) + sizeof( size_t ) );
   void * aligned_ptr = create_aligned_ptr_nec( ptr );
   (reinterpret_cast<T **>(aligned_ptr))[-1] = static_cast< T*>( malloc_ptr );
   return static_cast< T* >(aligned_ptr);
}

template< typename T >
T * nec_vecmalloc() {
   return nec_malloc<T>( NEC_VECSIZE/sizeof(T) );
}

size_t get_size_with_alignment_padding_intel( size_t p_SizeOfMemChunk ) {
   return p_SizeOfMemChunk+INTEL_ALIGNMENT_MINUS_ONE_BYTE;
}

void* create_aligned_ptr_intel(void* p_Ptr) {
   size_t ptrToSizeT = reinterpret_cast<size_t>(p_Ptr);
   size_t offset = ptrToSizeT & INTEL_ALIGNMENT_MINUS_ONE_BYTE;
   return reinterpret_cast<void*>( ptrToSizeT + INTEL_ALIGNMENT_BYTE + (-offset | INTEL_ALIGNMENT_TWOS_COMPLEMENT) );
}


template< typename T >
T * intel_malloc( size_t count ) {
   size_t datasize = get_size_with_alignment_padding_intel( ( count * sizeof( T ) ) + sizeof( T* ) );
   void * malloc_ptr = malloc( datasize );
   void * ptr = static_cast< void * >( static_cast< char * >( malloc_ptr ) + sizeof( size_t ) );
   void * aligned_ptr = create_aligned_ptr_intel( ptr );
   (reinterpret_cast<T **>(aligned_ptr))[-1] = static_cast< T*>( malloc_ptr );
   return static_cast< T* >(aligned_ptr);
}

template< typename T >
T * intel_vecmalloc() {
   return intel_malloc<T>( INTEL_VECSIZE/sizeof(T) );
}


template< typename T >
void afree( T * ptr ) {
   free( (reinterpret_cast<T ** >(ptr))[-1] );
}


#endif

