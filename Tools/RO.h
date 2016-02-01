// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _RO
#define _RO

#include "Tools/CBC-MAC.h"

/* The PRNG in random.h just takes a seed from somewhere
 * and produces a stream.
 *
 * The following, which uses AES, implements a "random oracle"
 * by CBC-MACing the input stream, then using this is a 
 * key in CTR mode to generate the output stream.
 *
 * Note that to get the same output the Update functions
 * must be called in the same way
 */

class RO
{
  CBC_MAC MC;

  public:

  RO() { MC.zero_key(); }
 
 
  void Reset() { MC.Reset(); }

  // Reset and apply update
  void Reset(const octetStream& inp) 
    { MC.Reset();
      MC.Update(inp);
    }
 
  void Update(const octetStream& inp) { MC.Update(inp); }

  void Output(octetStream& ans,int len)
    { PRNG G;
      octet seed[AES_BLK_SIZE];
      MC.Finalize(seed);
      G.SetSeed(seed);
      G.get_octetStream(ans,len);
    }

  void Call(octetStream& inp,octetStream& out,int len)
    {
      Reset(inp);
      Output(out,len);
    }
};

#endif
