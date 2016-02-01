// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _Commit
#define _Commit

/* Define the hash based commitment scheme */

#include "Tools/octetStream.h"

void Commit(octetStream& comm,octetStream& open,const octetStream& data);

bool Open(octetStream& data,const octetStream& comm,const octetStream& open);

#endif

