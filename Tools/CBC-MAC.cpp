// (C) 2016 University of Bristol. See LICENSE.txt


#include "Tools/CBC-MAC.h"
#include <wmmintrin.h> 

void CBC_MAC::zero_key()
{
  octet key[AES_BLK_SIZE];
  memset(key,0,AES_BLK_SIZE*sizeof(octet));
  Set_Key(key);
}

CBC_MAC::CBC_MAC()
{
  useC=(Check_CPU_support_AES()==0);
  zero_key();
}


void CBC_MAC::Set_Key(octet key[AES_BLK_SIZE])
{
  if (useC)
    { aes_schedule(KeyScheduleC,key); }
  else
    { aes_schedule(KeySchedule,key); }
  Reset();
}

void CBC_MAC::Reset()
{
  memset(state,0,AES_BLK_SIZE*sizeof(octet));
  buff_length=0;
}

void CBC_MAC::Update(const octetStream& s)
{
  Update(s.get_data(),s.get_length());
}

void CBC_MAC::Update(const octet* data,int length)
{
  // If stream+buff_length < AES_BLK_SIZE then just do a xor in
  if ((length+buff_length)<AES_BLK_SIZE)
    { for (int i=0; i<length; i++)
        { state[i+buff_length]^=data[i]; }
      buff_length+=length;
    }
  else
    { octet output[AES_BLK_SIZE];
      int pos=0;
      // First deal with the bit in the buffer
      if (buff_length!=0)
      {
          for (int i=0; i<AES_BLK_SIZE-buff_length; i++)
	        {
            state[i+buff_length]^=data[pos];
	          pos++;
          }
          buff_length=0;
          if (useC)
            { aes_encrypt(output,state,KeyScheduleC); }
          else
            { aes_encrypt(output,state,KeySchedule); }
          memcpy(state,output,AES_BLK_SIZE*sizeof(octet));
	    }
      // Now deal with the main bit of s
      if (useC)
      { while ((pos+AES_BLK_SIZE)<=length)
        {
          for (int i=0; i<AES_BLK_SIZE; i+=sizeof(word))
          {
            *(word*)(state+i)^=*(word*)(data+i+pos);
          }
          aes_encrypt(output,state,KeyScheduleC);
          pos+=AES_BLK_SIZE;
        }
        memcpy(state,output,AES_BLK_SIZE*sizeof(octet));
      }
      else
      {
        __m128i tmp =  _mm_loadu_si128 ((__m128i*)state);
        while ((pos+AES_BLK_SIZE)<=length)
        {
          __m128i tmp2 = _mm_loadu_si128 ((__m128i*)(data+pos));
          tmp = _mm_xor_si128 (tmp, tmp2);
          tmp = aes_encrypt(tmp,KeySchedule);
          pos+=AES_BLK_SIZE;
        }
        _mm_storeu_si128 ((__m128i*)state,tmp);
      }
      // Now buffer the remaining stuff until next time
      buff_length=length-pos;
      for (int i=0; i<buff_length; i++)
        { state[i]^=data[i+pos]; }
    }
}


void CBC_MAC::Finalize(octet output[AES_BLK_SIZE])
{ 
  // Deal with remaining stuff in buffer if needed
  if (buff_length!=0)
    { if (useC)
        { aes_encrypt(output,state,KeyScheduleC); }
      else
        { aes_encrypt(output,state,KeySchedule); }
    }
  else
    { memcpy(output,state,AES_BLK_SIZE*sizeof(octet)); }
}

