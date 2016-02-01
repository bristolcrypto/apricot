// (C) 2016 University of Bristol. See LICENSE.txt

#include "OT/BaseOT.h"
#include "Tools/random.h"
#include "Tools/CBC-MAC.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;


const char* role_to_str(OT_ROLE role)
{
    if (role == RECEIVER)
        return "RECEIVER";
    if (role == SENDER)
        return "SENDER";
    return "BOTH";
}

OT_ROLE INV_ROLE(OT_ROLE role)
{
    if (role == RECEIVER)
        return SENDER;
    if (role == SENDER)
        return RECEIVER;
    else
        return BOTH;
}

void send_if_ot_sender(const TwoPartyPlayer* P, vector<octetStream>& os, OT_ROLE role)
{
    if (role == SENDER)
    {
        //cout << "sending as sender\n";
        P->send(os[0]);
    }
    else if (role == RECEIVER)
    {
        //cout << "receiving as receiver\n";
        P->receive(os[1]);
    }
    else
    {
        //cout << "receiving as both\n";
        // both sender + receiver
        P->send_receive_player(os);
    }
}

void send_if_ot_receiver(const TwoPartyPlayer* P, vector<octetStream>& os, OT_ROLE role)
{
    if (role == RECEIVER)
    {
        //cout << "sending as receiver\n";
        P->send(os[0]);
    }
    else if (role == SENDER)
    {
        //cout << "receiving as sender\n";
        P->receive(os[1]);
    }
    else
    {
        //cout << "sending as both\n";
        // both
        P->send_receive_player(os);
    }
}

void BaseOT::exec_base()
{
  // Implement OT here
}

void BaseOT::set_seeds()
{
    for (int i = 0; i < nOT; i++)
    {
        // Set PRG seeds
        if (ot_role & SENDER)
        {
            G_sender[i][0].SetSeed(sender_inputs[i][0].get_ptr());
            G_sender[i][1].SetSeed(sender_inputs[i][1].get_ptr());
        }
        if (ot_role & RECEIVER)
        {
            G_receiver[i].SetSeed(receiver_outputs[i].get_ptr());
        }
    }
}

void BaseOT::extend_length()
{
    for (int i = 0; i < nOT; i++)
    {
        if (ot_role & SENDER)
        {
            sender_inputs[i][0].randomize(G_sender[i][0]);
            sender_inputs[i][1].randomize(G_sender[i][1]);
        }
        if (ot_role & RECEIVER)
        {
            receiver_outputs[i].randomize(G_receiver[i]);
        }
    }
}


void BaseOT::check()
{
    vector<octetStream> os(2);
    BitVector tmp_vector(8 * AES_BLK_SIZE);


    for (int i = 0; i < nOT; i++)
    {
        if (ot_role == SENDER)
        {
            // send both inputs over
            sender_inputs[i][0].pack(os[0]);
            sender_inputs[i][1].pack(os[0]);
            P->send(os[0]);
        }
        else if (ot_role == RECEIVER)
        {
            P->receive(os[1]);
        }
        else
        {
            // both sender + receiver
            sender_inputs[i][0].pack(os[0]);
            sender_inputs[i][1].pack(os[0]);
            P->send_receive_player(os);
        }
        if (ot_role & RECEIVER)
        {
            tmp_vector.unpack(os[1]);
        
            if (receiver_inputs[i] == 1)
            {
                tmp_vector.unpack(os[1]);
            }
            if (!tmp_vector.equals(receiver_outputs[i]))
            {
                cerr << "Incorrect OT\n";
                exit(1);
            }
        }
        os[0].reset_write_head();
        os[1].reset_write_head();
    }
}


void FakeOT::exec_base()
{
    PRNG G;
    G.ReSeed();
    vector<octetStream> os(2);
    vector<BitVector> bv(2);

    if (ot_role & RECEIVER)
        for (int i = 0; i < nOT; i++)
            // Generate my receiver inputs
            receiver_inputs[i] = G.get_uchar()&1;

    if (ot_role & SENDER)
        for (int i = 0; i < nOT; i++)
            for (int j = 0; j < 2; j++)
            {
                sender_inputs[i][j].randomize(G);
                sender_inputs[i][j].pack(os[0]);
            }

    send_if_ot_sender(P, os, ot_role);

    if (ot_role & RECEIVER)
        for (int i = 0; i < nOT; i++)
        {
            for (int j = 0; j < 2; j++)
                bv[j].unpack(os[1]);
            receiver_outputs[i] = bv[receiver_inputs[i]];
        }

    set_seeds();
}
