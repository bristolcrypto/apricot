// (C) 2016 University of Bristol. See LICENSE.txt


#include "Player.h"
#include "Exceptions/Exceptions.h"

// Use printf rather than cout so valgrind can detect thread issues

void Names::init(int player,int pnb,const char* servername)
{  
  player_no=player;
  portnum_base=pnb;
  setup_names(servername);
}


void Names::init(int player,int pnb,vector<octet*> Nms)
{
  player_no=player;
  portnum_base=pnb;
  nplayers=Nms.size();
  names.resize(nplayers);
  for (int i=0; i<nplayers; i++)
    { names[i]=(char*)Nms[i]; }
}


void Names::init(int player,int pnb,vector<string> Nms)
{
  player_no=player;
  portnum_base=pnb;
  nplayers=Nms.size();
  names=Nms;
}


void Names::setup_names(const char *servername)
{
  int socket_num;
  set_up_client_socket(socket_num,servername,portnum_base+player_no); 

  int inst=-1; // wait until instruction to start.
  while (inst != GO) { receive(socket_num, inst); }

  // Send my name
  octet my_name[512];
  memset(my_name,0,512*sizeof(octet));
  gethostname((char*)my_name,512);
  printf("My Name = %s\n",my_name);
  send(socket_num,my_name,512);
  cout << "My number = " << player_no << endl;
  
  // Now get the set of names
  int i;
  receive(socket_num,nplayers);
  cout << nplayers << " players\n";
  names.resize(nplayers);
  for (i=0; i<nplayers; i++)
    { octet* tmp=new octet[512];
      receive(socket_num,tmp,512);
      names[i]=(char*)tmp;
      cout << "Player " << i << " is running on machine " << names[i] << endl;
    }
  close_client_socket(socket_num);
}


TwoPartyPlayer::TwoPartyPlayer(const Names& Nms, int other_player, int pn_offset)
{
  nplayers = Nms.nplayers;
  player_no = Nms.player_no;
  this->other_player = other_player;
  setup_sockets(Nms.names, Nms.portnum_base+pn_offset); 
  blk_SHA1_Init(&ctx);
}


TwoPartyPlayer::~TwoPartyPlayer()
{ 
  /* Close down the sockets */
  if (player_no < other_player)
  {
      close_server_socket(sockets[0],msocket[0]);
  }
  else
  {
      close_client_socket(sockets[0]);
  }
}

void TwoPartyPlayer::setup_sockets(const vector<string>& names,int portnum_base)
{
  sockets_open = true;
  sockets.resize(1);
  msocket.resize(1);
  sockaddr_in dest;
  int pn = portnum_base;

  printf("pn base = %d, player_no = %d, other_player = %d\n", portnum_base, player_no, other_player);

  if (player_no < other_player)
  {
    printf("Setting up server on %d\n", pn);
    set_up_server_socket(dest, sockets[0], msocket[0], pn);
  }
  else
  {
    printf("Setting up client to %s %d\n", names[other_player].c_str(), pn);
    set_up_client_socket(sockets[0], names[other_player].c_str(), pn);
  }
}

void TwoPartyPlayer::send(octetStream& o) const
{
  o.Send(sockets[0]);
}

void TwoPartyPlayer::receive(octetStream& o) const
{
  o.reset_write_head();
  o.Receive(sockets[0]);
}

void TwoPartyPlayer::send_receive_player(vector<octetStream>& o) const
{
  {
    if (other_player < player_no)
    {
      o[0].Send(sockets[0]);
      o[1].reset_write_head();
      o[1].Receive(sockets[0]);
    }
    else
    {
      o[1].reset_write_head();
      o[1].Receive(sockets[0]);
      o[0].Send(sockets[0]);
    }
  }
}


