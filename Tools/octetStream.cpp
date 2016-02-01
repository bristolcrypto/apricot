// (C) 2016 University of Bristol. See LICENSE.txt


#include <fcntl.h>

#include "octetStream.h"
#include <string.h>
#include "Networking/sockets.h"
#include "Tools/sha1.h"
#include "Exceptions/Exceptions.h"
#include "Networking/data.h"


void octetStream::resize(int l)
{
  if (l<mxlen) { return; }
  l=2*l;      // Overcompensate in the resize to avoid calling this a lot
  octet* nd=new octet[l];
  memcpy(nd,data,len*sizeof(octet));
  delete[] data;
  data=nd;
  mxlen=l;
}


void octetStream::assign(const octetStream& os)
{
  if (os.len>=mxlen)
    { delete[] data; 
      mxlen=os.mxlen;  
      data=new octet[mxlen];
    }
  len=os.len;
  memcpy(data,os.data,len*sizeof(octet));
  ptr=os.ptr;
}


octetStream::octetStream(int maxlen)
{
  mxlen=maxlen; len=0; ptr=0;
  data=new octet[mxlen];
}


octetStream::octetStream(const octetStream& os)
{
  mxlen=os.mxlen;
  len=os.len;
  data=new octet[mxlen];
  memcpy(data,os.data,len*sizeof(octet));
  ptr=os.ptr;
}


void octetStream::hash(octetStream& output) const
{
  blk_SHA_CTX ctx;
  blk_SHA1_Init(&ctx);
  blk_SHA1_Update(&ctx,data,len);
  blk_SHA1_Final(output.data,&ctx);
  output.len=HASH_SIZE;
}


octetStream octetStream::hash() const
{
  octetStream h(HASH_SIZE);
  hash(h);
  return h;
}



bool octetStream::equals(const octetStream& a) const
{
  if (len!=a.len) { return false; }
  for (int i=0; i<len; i++)
    { if (data[i]!=a.data[i]) { return false; } }
  return true;
}


void octetStream::append_random(int num)
{
  resize(len+num);
  int randomData = open("/dev/urandom", O_RDONLY);
  read(randomData, data+len, num*sizeof(unsigned char));
  close(randomData);
  len+=num;
}


void octetStream::concat(const octetStream& os)
{
  resize(len+os.len);
  memcpy(data+len,os.data,os.len*sizeof(octet));
  len+=os.len;
}


void octetStream::store_bytes(octet* x, const int l)
{
  resize(len+4+l); 
  encode_length(data+len,l); len+=4;
  memcpy(data+len,x,l*sizeof(octet));
  len+=l;
}

void octetStream::get_bytes(octet* ans, int& length)
{
  length=decode_length(data+ptr); ptr+=4;
  memcpy(ans,data+ptr,length*sizeof(octet));
  ptr+=length;
}

void octetStream::store(unsigned int l)
{
  resize(len+4);
  encode_length(data+len,l); 
  len+=4;
}


void octetStream::get(unsigned int& l)
{
  l=decode_length(data+ptr);
  ptr+=4;
}




void octetStream::Send(int socket_num) const
{
  octet blen[4];
  encode_length(blen,len);
  send(socket_num,blen,4);
  send(socket_num,data,len);
}

void octetStream::Receive(int socket_num)
{
  octet blen[4];
  receive(socket_num,blen,4);

  int nlen=decode_length(blen);
  len=0;
  resize(nlen);
  len=nlen;

  receive(socket_num,data,len);
}

ostream& operator<<(ostream& s,const octetStream& o)
{
  for (int i=0; i<o.len; i++)
    { int t0=o.data[i]&15;
      int t1=o.data[i]>>4;
      s << hex << t1 << t0 << dec;
    }
  return s;
}




