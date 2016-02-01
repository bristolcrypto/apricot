// (C) 2016 University of Bristol. See LICENSE.txt

#include "MMO.h"
#include "Tools/int.h"
#include <unistd.h>


void MMO::zeroIV()
{
    octet key[AES_BLK_SIZE];
    memset(key,0,AES_BLK_SIZE*sizeof(octet));
    setIV(key);
}


void MMO::setIV(octet key[AES_BLK_SIZE])
{
    aes_schedule(IV,key);
}


void MMO::hashOneBlock(octet* output, octet* input)
{
    __m128i in = _mm_loadu_si128((__m128i*)input);
    __m128i ct = aes_encrypt(in, IV);
//    __m128i out = ct ^ in;
    _mm_storeu_si128((__m128i*)output, ct);
}

template <>
void MMO::hashBlockWise<128>(octet* output, octet* input)
{
    for (int i = 0; i < 16; i++)
        ecb_aes_128_encrypt<8>(&((__m128i*)output)[i*8], &((__m128i*)input)[i*8], IV);
}
