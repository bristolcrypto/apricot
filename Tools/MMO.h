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

#endif /* TOOLS_MMO_H_ */
