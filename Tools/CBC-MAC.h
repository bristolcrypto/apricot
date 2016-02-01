// (C) 2016 University of Bristol. See LICENSE.txt


#ifndef _CBC_MAC
#define _CBC_MAC

/* This defines CBC-MAC using AES-128
 *
 * The version of CBC-MAC is with zero padding upto the block
 * boundary. As such this should not be used as a main MAC function.
 * The main purpose is to define a PRG via  
 *       F(M) = PRNG(CBC_MAC(key=0,message=M))
 *
 * If combined with the PRNG in random.h in block cipher mode the
 * output of this PRNG will be essentially counter mode under the
 * key defined by the output of the MAC. It is common to model this
 * construction as a random oracle.
 *
 */

#include "Tools/aes.h"
#include "Tools/octetStream.h"
#include "Tools/random.h"

class CBC_MAC
{
  bool useC;

  // Two types of key schedule for the different implementations 
  // of AES
  uint  KeyScheduleC[44];
  octet KeySchedule[176]  __attribute__((aligned (16)));

  octet state[AES_BLK_SIZE];
  int buff_length;

  public:

  void zero_key(); 

  // Default is the zero key
  CBC_MAC(); 

  void Set_Key(octet key[AES_BLK_SIZE]);
  void Reset();
  void Update(const octetStream& s);
  void Update(const octet* data,int length);
  void Finalize(octet output[AES_BLK_SIZE]);
};

#endif

