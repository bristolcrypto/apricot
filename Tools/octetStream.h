// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _octetStream
#define _octetStream


#include "Tools/int.h"
#include <string.h>
#include <vector>
#include <stdio.h>
#include <iostream>
using namespace std;


class octetStream
{
  int len,mxlen,ptr;  // len is the "write head", ptr is the "read head"
  octet *data;

  public:

  void resize(int l);

  void assign(const octetStream& os);

  octetStream(int maxlen=512);
  octetStream(const octetStream& os);
  octetStream& operator=(const octetStream& os)
    { if (this!=&os) { assign(os); }
      return *this;
    }
  ~octetStream() { delete[] data; }
  
  int get_ptr() const     { return ptr; }
  int get_length() const  { return len; }
  octet* get_data() const { return data; }

  octetStream hash()   const;
  // output must have length at least HASH_SIZE
  void hash(octetStream& output)   const;

  void concat(const octetStream& os);

  void reset_read_head()  { ptr=0; }
  /* If we reset write head then we should reset the read head as well */
  void reset_write_head() { len=0; ptr=0; }

  // Move len back num
  void rewind_write_head(int num) { len-=num; }

  bool equals(const octetStream& a) const;

  /* Append NUM random bytes from dev/random */
  void append_random(int num);

  // Append with no padding for decoding
  void append(const octet* x,const int l); 
  // Read l octets, with no padding for decoding
  void consume(octet* x,const int l);

  /* Now store and restore different types of data (with padding for decoding) */

  void store_bytes(octet* x, const int l); //not really "bytes"...
  void get_bytes(octet* ans, int& l);      //Assumes enough space in ans

  void store(unsigned int a);
  void store(int a)   { store((unsigned int) a); }
  void get(unsigned int& a);
  void get(int& a) { get((unsigned int&) a); }

  void consume(octetStream& s,int l)
    { s.resize(l);
      consume(s.data,l);
      s.len=l;
    }

  void Send(int socket_num) const;
  void Receive(int socket_num);

  friend ostream& operator<<(ostream& s,const octetStream& o);
  friend class PRNG;
};


inline void octetStream::append(const octet* x, const int l)
{
  if (len+l>mxlen)
    resize(len+l);
  memcpy(data+len,x,l*sizeof(octet));
  len+=l;
}


inline void octetStream::consume(octet* x,const int l)
{
  memcpy(x,data+ptr,l*sizeof(octet));
  ptr+=l;
}


#endif

