// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef TOOLS_MMO_H_
#define TOOLS_MMO_H_

#include "Tools/aes.h"

// Matyas-Meyer-Oseas hashing
class MMO
{
    octet IV[176]  __attribute__((aligned (16)));

public:
    MMO() { zeroIV(); }
    void zeroIV();
    void setIV(octet key[AES_BLK_SIZE]);
    void hashOneBlock(octet* output, octet* input);
    template <int N>
    void hashBlockWise(octet* output, octet* input);
    void outputOneBlock(octet* output);
};


inline void MMO::hashOneBlock(octet* output, octet* input)
{
    __m128i in = _mm_loadu_si128((__m128i*)input);
    __m128i ct = aes_encrypt(in, IV);
//    __m128i out = ct ^ in;
    _mm_storeu_si128((__m128i*)output, ct);
}

#endif /* TOOLS_MMO_H_ */
