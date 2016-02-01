// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _Player
#define _Player

/* Class to create a player, for KeyGen, Offline and Online phases.
 *
 * Basically handles connection to the server to obtain the names
 * of the other players. Plus sending and receiving of data
 *
 */

#include <vector>
#include <iostream>
using namespace std;

#include "Tools/octetStream.h"
#include "Networking/sockets.h"
#include "Tools/sha1.h"

/* Class to get the names off the server */
class Names
{
  vector<string> names;
  int nplayers;
  int portnum_base;
  int player_no;

  void setup_names(const char *servername);

  public:

  // Usual setup names
  void init(int player,int pnb,const char* servername);
  Names(int player,int pnb,const char* servername)
    { init(player,pnb,servername); }
  // Set up names when we KNOW who we are going to be using before hand
  void init(int player,int pnb,vector<octet*> Nms);
  Names(int player,int pnb,vector<octet*> Nms)
    { init(player,pnb,Nms); }
  void init(int player,int pnb,vector<string> Nms);
  Names(int player,int pnb,vector<string> Nms)
    { init(player,pnb,Nms); }
  Names()  { ; }
  
  int num_players() const { return nplayers; }
  int my_num() const { return player_no; }
  const string get_name(int i) { return names[i]; }

  friend class Player;
  friend class TwoPartyPlayer;
};


class PlayerBase
{
protected:
  vector<int> sockets;
  vector<int> msocket;
   
  void setup_sockets(const vector<octet*>& names,int portnum_base);

  int player_no;
  int nplayers;

  mutable blk_SHA_CTX ctx;

  public:

  int num_players() const { return nplayers; }
  int my_num() const { return player_no; }
  int socket(int i) const { return sockets[i]; }
};


class TwoPartyPlayer : public PlayerBase
{
protected:
  // setup sockets for comm. with only one other player
  void setup_sockets(const vector<string>& names,int portnum_base);

  int other_player;
  bool sockets_open;

public:
  TwoPartyPlayer(const Names& Nms, int other_player, int pn_offset=0);
  ~TwoPartyPlayer();

  void send(octetStream& o) const;
  void receive(octetStream& o) const;

  /* Send and receive to/from the other player
   *  - o[0] contains my data, received data put in o[1]
   */
  void send_receive_player(vector<octetStream>& o) const;
};

#endif
