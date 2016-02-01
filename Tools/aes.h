// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef __AES_H
#define __AES_H

#include <wmmintrin.h>

#include "Networking/data.h"

typedef unsigned int  uint;

#define AES_BLK_SIZE 16

/************* C Version *************/
// Key Schedule
inline void aes_schedule( uint* RK, octet* K )
{ *RK = 0; *K = 0; }

// Encryption Function 
inline void aes_encrypt( octet* C, octet* M, uint* RK )
{ *C = 0; *M = 0; *RK = 0; }


/*********** M-Code Version ***********/
// Check can support this
int Check_CPU_support_AES();
// Key Schedule 
void aes_128_schedule( octet* key, const octet* userkey );
void aes_192_schedule( octet* key, const octet* userkey );
void aes_256_schedule( octet* key, const octet* userkey );

inline void aes_schedule( octet* key, const octet* userkey )
{ aes_128_schedule(key,userkey); }


// Encryption Function 
void aes_128_encrypt( octet* C, const octet* M,const octet* RK );
void aes_192_encrypt( octet* C, const octet* M,const octet* RK );
void aes_256_encrypt( octet* C, const octet* M,const octet* RK );

__attribute__((optimize("unroll-loops")))
inline __m128i aes_128_encrypt(__m128i in, const octet* key)
{ __m128i& tmp = in;
  tmp = _mm_xor_si128 (tmp,((__m128i*)key)[0]);
  int j;
  for(j=1; j <10; j++)
      { tmp = _mm_aesenc_si128 (tmp,((__m128i*)key)[j]); }
  tmp = _mm_aesenclast_si128 (tmp,((__m128i*)key)[j]);
  return tmp;
}

template <int N>
__attribute__((optimize("unroll-loops")))
inline void ecb_aes_128_encrypt(__m128i* out, __m128i* in, const octet* key)
{
    __m128i tmp[N];
    for (int i = 0; i < N; i++)
        tmp[i] = _mm_xor_si128 (in[i],((__m128i*)key)[0]);
    int j;
    for(j=1; j <10; j++)
        for (int i = 0; i < N; i++)
            tmp[i] = _mm_aesenc_si128 (tmp[i],((__m128i*)key)[j]);
    for (int i = 0; i < N; i++)
        out[i] = _mm_aesenclast_si128 (tmp[i],((__m128i*)key)[j]);
}

template <int N>
inline void ecb_aes_128_encrypt(__m128i* out, const __m128i* in, const octet* key, const int* indices)
{
    __m128i tmp[N];
    for (int i = 0; i < N; i++)
        tmp[i] = in[indices[i]];
    ecb_aes_128_encrypt<N>(tmp, tmp, key);
    for (int i = 0; i < N; i++)
        out[indices[i]] = tmp[i];
}

inline void aes_encrypt( octet* C, const octet* M,const octet* RK )
{ aes_128_encrypt(C,M,RK); }

inline __m128i aes_encrypt( __m128i M,const octet* RK )
{ return aes_128_encrypt(M,RK); }


#endif

