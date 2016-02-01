// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _BITVECTOR
#define _BITVECTOR

/* Vector of bits */

#include <iostream>
#include <vector>
using namespace std;
#include <stdlib.h>

#include "Exceptions/Exceptions.h"
#include "Networking/data.h"
// just for util functions
#include "Tools/int.h"

class PRNG;
class octetStream;


class BitVector
{
    octet* bytes;

    size_t nbytes;
    size_t nbits;
    size_t length;

    public:

    void assign(const BitVector& K)
    {
        if (nbits != K.nbits)
        {
            resize(K.nbits);
        }
        memcpy(bytes, K.bytes, nbytes);
    }
    void assign_zero()
    {
        memset(bytes, 0, nbytes);
    }
    // erases original contents
    void resize(size_t new_nbits)
    {
        if (nbits != new_nbits)
        {
            int new_nbytes = DIV_CEIL(new_nbits,8);
            octet* tmp = new octet[new_nbytes];

            nbits = new_nbits;
            nbytes = new_nbytes;
            delete[] bytes;
            bytes = tmp;
            /*
            // use realloc to preserve original contents
            if (new_nbits < nbits)
            {
                memcpy(tmp, bytes, new_nbytes);
            }
            else
            {
                memset(tmp, 0, new_nbytes);
                memcpy(tmp, bytes, nbytes);   
            }*/

            // realloc may fail on size 0
            /*if (new_nbits == 0)
            {
                free(bytes);
                bytes = (octet*) malloc(0);//new octet[0];
                //free(bytes);
                return;
            }
            bytes = (octet*)realloc(bytes, nbytes);
            if (bytes == NULL)
            {
                cerr << "realloc failed\n";
                exit(1);
            }*/
            /*delete[] bytes;
            nbits = new_nbits;
            nbytes = DIV_CEIL(nbits, 8);
            bytes = new octet[nbytes];*/
        }
    }
    unsigned int size() const { return nbits; }
    unsigned int size_bytes() const { return nbytes; }
    octet* get_ptr() { return bytes; }

    BitVector(size_t n=128)
    {
        nbits = n;
        nbytes = DIV_CEIL(nbits, 8);
        //bytes = (octet*) malloc(nbytes);
        bytes = new octet[nbytes];
        length = n;
        assign_zero();
    }
    BitVector(const BitVector& K)
    {
        bytes = (octet*) malloc(K.nbytes);
        //bytes = new octet[K.nbytes];
        nbytes = K.nbytes;
        nbits = K.nbits;
        assign(K);
    }
    ~BitVector() {
        //cout << "Destroy, size = " << nbytes << endl;
        delete[] bytes;
    }
    BitVector& operator=(const BitVector& K)
    {
        if (this!=&K) { assign(K); }
        return *this;
    }

    octet get_byte(int i) const { return bytes[i]; }

    void set_byte(int i, octet b) { bytes[i] = b; }

    // get the i-th 64-bit word
    word get_word(int i) const { return *(word*)(bytes + i*8); }

    void set_word(int i, word w)
    {
        int offset = i * sizeof(word);
        memcpy(bytes + offset, (octet*)&w, sizeof(word));
    }

    int  get_bit(int i) const
      {
        return (bytes[i/8] >> (i % 8)) & 1;
      }
    void set_bit(int i,unsigned int a)
    {
        int j = i/8, k = i&7;
        if (a==1)
          { bytes[j] |= (octet)(1UL<<k); }
        else
  	     { bytes[j] &= (octet)~(1UL<<k); }
    }

    void add(const BitVector& A, const BitVector& B)
    {
        if (A.nbits != B.nbits)
          { throw invalid_length(); }
        resize(A.nbits);
        for (unsigned int i=0; i < nbytes; i++)
  	    {
            bytes[i] = A.bytes[i] ^ B.bytes[i];
        }
    }

    void add(const BitVector& A)
    {
        if (nbits != A.nbits)
          { throw invalid_length(); }
        for (unsigned int i = 0; i < nbytes; i++)
  	    {
            bytes[i] ^= A.bytes[i];
        }
    }
    
    bool equals(const BitVector& K) const
    {
        if (nbits != K.nbits)
          { throw invalid_length(); }
        for (unsigned int i = 0; i < nbytes; i++)
          { if (bytes[i] != K.bytes[i]) { return false; } }
        return true;
    }

    void randomize(PRNG& G);
    // randomize bytes a, ..., a+nb-1
    void randomize_at(int a, int nb, PRNG& G);

    void output(ostream& s,bool human) const;
    void input(istream& s,bool human);

    // Pack and unpack in native format
    //   i.e. Dont care about conversion to human readable form
    void pack(octetStream& o) const;
    void unpack(octetStream& o);

    string str()
    {
        stringstream ss;
        ss << hex;
        for(size_t i(0);i < nbytes;++i)
            ss << (int)bytes[i] << " ";
        return ss.str();
    }
};

#endif
