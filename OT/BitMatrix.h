// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef OT_BITMATRIX_H_
#define OT_BITMATRIX_H_

#include <vector>
#include <emmintrin.h>
#include <immintrin.h>

#include "BitVector.h"
#include "Tools/random.h"
#include "Tools/MMO.h"

using namespace std;

union square128 {
#ifdef __AVX2__
    __m256i doublerows[64];
#endif
    __m128i rows[128];
    octet bytes[128][16];
    int16_t doublebytes[128][8];
    int32_t words[128][4];

    bool get_bit(int x, int y)
    { return (bytes[x][y/8] >> (y % 8)) & 1; }

    void set_zero();

    square128& operator^=(square128& other);
    square128& operator^=(__m128i* other);
    square128& operator^=(BitVector& other);
    bool operator==(square128& other);

    void randomize(PRNG& G);
    void randomize(int row, PRNG& G);
    void conditional_xor(BitVector& conditions, square128& other);
    void transpose();
    void hash_row_wise(MMO& mmo, square128& input);

    void check_transpose(square128& dual, int i, int k);
    void check_transpose(square128& dual);
    void print(int i, int k);
    void print();
    void print_octets();
    void print_doublerows();

    // Pack and unpack in native format
    //   i.e. Dont care about conversion to human readable form
    void pack(octetStream& o) const;
    void unpack(octetStream& o);
};

// allocator to counter GCC bug
template <typename _Tp, int ALIGN>
class aligned_allocator : public std::allocator<_Tp>
{
public:
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;

    template<typename _Tp1>
    struct rebind
    { typedef aligned_allocator<_Tp1, ALIGN> other; };

    _Tp*
    allocate(size_t __n, const void* = 0)
    {
        if (__n > this->max_size())
            std::__throw_bad_alloc();

        _Tp* res = 0;
        posix_memalign((void**)&res, ALIGN, __n * sizeof(_Tp));
        if (res == 0)
            std::__throw_bad_alloc();
        return res;
    }
};

class BitMatrixSlice;

class BitMatrix
{
public:
    vector< square128, aligned_allocator<square128, 32> > squares;

    BitMatrix() {}
    BitMatrix(int length);
    void resize(int length);

    BitMatrix& operator^=(BitMatrix& other);
    BitMatrix& operator^=(BitMatrixSlice& other);
    BitMatrix& operator^=(BitVector& other);
    bool operator==(BitMatrix& other);
    bool operator!=(BitMatrix& other);

    void randomize(PRNG& G);
    void randomize(int row, PRNG& G);
    void conditional_xor(BitVector& conditions, BitMatrix& other);
    void transpose();

    void check_transpose(BitMatrix& dual);
    void print_side_by_side(BitMatrix& other);
    void print_conditional(BitVector& conditions);

    // Pack and unpack in native format
    //   i.e. Dont care about conversion to human readable form
    void pack(octetStream& o) const;
    void unpack(octetStream& o);
};

class BitMatrixSlice
{
    friend class BitMatrix;

    BitMatrix& bm;
    size_t start, size, end;

public:
    BitMatrixSlice(BitMatrix& bm, size_t start, size_t size);

    BitMatrixSlice& operator^=(BitMatrixSlice& other);
    BitMatrixSlice& operator^=(BitVector& other);

    void randomize(int row, PRNG& G);
    void conditional_xor(BitVector& conditions, BitMatrix& other);
    void transpose();

    // Pack and unpack in native format
    //   i.e. Dont care about conversion to human readable form
    void pack(octetStream& o) const;
    void unpack(octetStream& o);
};

#endif /* OT_BITMATRIX_H_ */
