// (C) 2016 University of Bristol. See LICENSE.txt


#include "Commit.h"
#include "random.h"

void Commit(octetStream& comm,octetStream& open,const octetStream& data)
{
  open=data;
  open.append_random(SEED_SIZE);
  comm=open.hash();
}


bool Open(octetStream& data,const octetStream& comm,const octetStream& open)
{
  octetStream h=open.hash();
  if (!h.equals(comm)) { return false; }
  data=open;
  data.rewind_write_head(SEED_SIZE);
  return true;
}

