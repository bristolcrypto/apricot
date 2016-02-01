// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _BASE_OT
#define _BASE_OT

/* This thread serves all base OTs to all other threads
 */

#include "Networking/Player.h"
#include "Tools/random.h"
#include "OT/BitVector.h"

// currently always assumes BOTH, i.e. do 2 sets of OT symmetrically,
// use bitwise & to check for role
enum OT_ROLE
{
	RECEIVER = 0x01,
	SENDER = 0x10,
	BOTH = 0x11
};

OT_ROLE INV_ROLE(OT_ROLE role);

const char* role_to_str(OT_ROLE role);
void send_if_ot_sender(const TwoPartyPlayer* P, vector<octetStream>& os, OT_ROLE role);
void send_if_ot_receiver(const TwoPartyPlayer* P, vector<octetStream>& os, OT_ROLE role);

class BaseOT
{
public:
	vector<int> receiver_inputs;
	vector< vector<BitVector> > sender_inputs;
	vector<BitVector> receiver_outputs;
	TwoPartyPlayer* P;

	BaseOT(int nOT, int ot_length, int other_player_num, TwoPartyPlayer* player, OT_ROLE role=BOTH)
		: nOT(nOT), ot_length(ot_length), other_player_num(other_player_num), P(player), ot_role(role)
	{
		receiver_inputs.resize(nOT);
		sender_inputs.resize(nOT, vector<BitVector>(2));
		receiver_outputs.resize(nOT);
		G_sender.resize(nOT, vector<PRNG>(2));
		G_receiver.resize(nOT);

		for (int i = 0; i < nOT; i++)
		{
			sender_inputs[i][0] = BitVector(8 * AES_BLK_SIZE);
			sender_inputs[i][1] = BitVector(8 * AES_BLK_SIZE);
			receiver_outputs[i] = BitVector(8 * AES_BLK_SIZE);
		}
	}
	virtual ~BaseOT() {}

	int length() { return ot_length; }

	// do the OTs
	virtual void exec_base();
	// use PRG to get the next ot_length bits
	void extend_length();
	void check();
protected:
	int nOT, ot_length;
	int other_player_num;
	Names* names;
	OT_ROLE ot_role;

	vector< vector<PRNG> > G_sender;
	vector<PRNG> G_receiver;

	bool is_sender() { return (bool) (ot_role & SENDER); }
	bool is_receiver() { return (bool) (ot_role & RECEIVER); }

	void set_seeds();
};

class FakeOT : public BaseOT
{
public:
   FakeOT(int nOT, int ot_length, int other_player_num, TwoPartyPlayer* player, OT_ROLE role=BOTH) :
       BaseOT(nOT, ot_length, other_player_num, player, role) {}
   void exec_base();
};

#endif
