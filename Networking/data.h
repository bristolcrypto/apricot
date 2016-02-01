// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _Data
#define _Data

#include <string.h>


typedef unsigned char octet;

// Assumes word is a 64 bit value
#ifdef WIN32
  typedef unsigned __int64 word;
#else
  typedef unsigned long word;
#endif

#define BROADCAST 0
#define ROUTE     1
#define TERMINATE 2
#define GO        3

void encode_length(octet *buff,int len);
int  decode_length(octet *buff);

#endif
