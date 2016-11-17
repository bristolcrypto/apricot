// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _OTEXTENSION
#define _OTEXTENSION

#include "OT/BaseOT.h"

#include "Exceptions/Exceptions.h"

#include "Networking/Player.h"

#include "Tools/RO.h"
#include "Tools/time-func.h"

#include <stdlib.h>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#define OTEXT_TIMER
//#define OTEXT_DEBUG
//#define VERBOSE

class OTExtensionBaseBase
{
public:
    vector< vector<BitVector> > senderOutput;
    vector<BitVector> receiverOutput;
    map<string,long long> times;

    virtual ~OTExtensionBaseBase() {}
    virtual void transfer(int nOTs, const BitVector& receiverInput) = 0;
};

template <class D>
class OTExtensionBase : public OTExtensionBaseBase
{
public:
    OTExtensionBase(int nbaseOTs, int baseLength,
                int nloops, int nsubloops,
                TwoPartyPlayer* player,
                BitVector& baseReceiverInput,
                vector< vector<BitVector> >& baseSenderInput,
                vector<BitVector>& baseReceiverOutput,
                OT_ROLE role=BOTH,
                bool passive=false)
        : passive_only(passive), nbaseOTs(nbaseOTs), baseLength(baseLength), nloops(nloops),
          nsubloops(nsubloops), ot_role(role), player(player), baseReceiverInput(baseReceiverInput)
    {
        G_sender.resize(nbaseOTs, vector<PRNG>(2));
        G_receiver.resize(nbaseOTs);

        // set up PRGs for expanding the seed OTs
        for (int i = 0; i < nbaseOTs; i++)
        {
            assert(baseSenderInput[i][0].size_bytes() >= AES_BLK_SIZE);
            assert(baseSenderInput[i][1].size_bytes() >= AES_BLK_SIZE);
            assert(baseReceiverOutput[i].size_bytes() >= AES_BLK_SIZE);

            if (ot_role & RECEIVER)
            {
                G_sender[i][0].SetSeed(baseSenderInput[i][0].get_ptr());
                G_sender[i][1].SetSeed(baseSenderInput[i][1].get_ptr());
            }
            if (ot_role & SENDER)
            {
                G_receiver[i].SetSeed(baseReceiverOutput[i].get_ptr());
            }

#ifdef OTEXT_DEBUG
            // sanity check for base OTs
            vector<octetStream> os(2);
            BitVector t0(128);

            if (ot_role & RECEIVER)
            {
                // send both inputs to test
                baseSenderInput[i][0].pack(os[0]);
                baseSenderInput[i][1].pack(os[0]);
            }
            send_if_ot_receiver(player, os, ot_role);

            if (ot_role & SENDER)
            {
                // sender checks results
                t0.unpack(os[1]);
                if (baseReceiverInput.get_bit(i) == 1)
                    t0.unpack(os[1]);
                if (!t0.equals(baseReceiverOutput[i]))
                {
                    cerr << "Incorrect base OT\n";
                    exit(1);
                }
            }
            

            os[0].reset_write_head();
            os[1].reset_write_head();
#endif
        }
    }

protected:
    bool passive_only;
    int nbaseOTs, baseLength, nloops, nsubloops;
    OT_ROLE ot_role;
    TwoPartyPlayer* player;
    vector< vector<PRNG> > G_sender;
    vector<PRNG> G_receiver;
    BitVector baseReceiverInput;

    void check_correlation(int nOTs,
        const BitVector& receiverInput);

    void check_iteration(__m128i delta, __m128i q, __m128i q2,
        __m128i t, __m128i t2, __m128i x);

};

class OTExtension : public OTExtensionBase<OTExtension>
{
protected:
    void hash_outputs(int nOTs, vector<BitVector>& receiverOutput);

public:
    void transfer(int nOTs, const BitVector& receiverInput);

    octet* get_receiver_output(int i);
    octet* get_sender_output(int choice, int i);

    struct block128 {
        vector<BitVector>& parent;
        int base;
        block128(vector<BitVector>& parent, int i) : parent(parent), base(128 * i) {}
        __m128i operator[](int j) { return _mm_loadu_si128((__m128i*)parent[base + j].get_ptr()); }
    };
    block128 get_receiver_output_128(int i) { return block128(receiverOutput, i); }
    block128 get_sender_output_128(int choice, int i) { return block128(senderOutput[choice], i); }
};

#endif
