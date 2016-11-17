// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _OTTOOLS
#define _OTTOOLS

#include <iostream>
using namespace std;

#include "Networking/Player.h"
#include "Tools/Commit.h"
#include "Tools/random.h"
#include "Exceptions/Exceptions.h"

#define SEED_SIZE_BYTES SEED_SIZE

/*
 * Generate a secure, random seed between 2 parties via commitment
 */
void random_seed_commit(octet* seed, const TwoPartyPlayer& player, int len);

/*
 * GF(2^128) multiplication using Intel instructions
 */
void gfmul128(__m128i a, __m128i b, __m128i *res);
// Without reduction
void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2);
void gfred128(__m128i a1, __m128i a2, __m128i *res);

//#if defined(__SSE2__)
/*
 * Convert __m128i to string of type T
 */
template <typename T>
string __m128i_toString(const __m128i var) {
    stringstream sstr;
    sstr << hex;
    const T* values = (const T*) &var;
    if (sizeof(T) == 1) {
        for (unsigned int i = 0; i < sizeof(__m128i); i++) {
            sstr << (int) values[i] << " ";
        }
    } else {
        for (unsigned int i = 0; i < sizeof(__m128i) / sizeof(T); i++) {
            sstr << values[i] << " ";
        }
    }
    return sstr.str();
}
//#endif

string word_to_bytes(const word w);

void shiftl128(word x1, word x2, word& res1, word& res2, size_t k);

inline void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2)
{
    __m128i tmp3, tmp4, tmp5, tmp6;

    tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
    tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
    tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
    tmp6 = _mm_clmulepi64_si128(a, b, 0x11);

    tmp4 = _mm_xor_si128(tmp4, tmp5);
    tmp5 = _mm_slli_si128(tmp4, 8);
    tmp4 = _mm_srli_si128(tmp4, 8);
    tmp3 = _mm_xor_si128(tmp3, tmp5);
    tmp6 = _mm_xor_si128(tmp6, tmp4);
    // initial mul now in tmp3, tmp6
    *res1 = tmp3;
    *res2 = tmp6;
}

#endif
