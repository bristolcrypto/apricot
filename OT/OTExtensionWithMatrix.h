// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef OT_OTEXTENSIONWITHMATRIX_H_
#define OT_OTEXTENSIONWITHMATRIX_H_

#include "OTExtension.h"
#include "BitMatrix.h"

class OTExtensionWithMatrix : public OTExtensionBase<OTExtensionWithMatrix>
{
public:
    vector<BitMatrix> senderOutputMatrices;
    BitMatrix receiverOutputMatrix;

    OTExtensionWithMatrix(int nbaseOTs, int baseLength,
                int nloops, int nsubloops,
                TwoPartyPlayer* player,
                BitVector& baseReceiverInput,
                vector< vector<BitVector> >& baseSenderInput,
                vector<BitVector>& baseReceiverOutput,
                OT_ROLE role=BOTH,
                bool passive=false)
    : OTExtensionBase<OTExtensionWithMatrix>(nbaseOTs, baseLength, nloops, nsubloops, player, baseReceiverInput,
            baseSenderInput, baseReceiverOutput, role, passive) {}

    void transfer(int nOTs, const BitVector& receiverInput);

    octet* get_receiver_output(int i);
    octet* get_sender_output(int choice, int i);
    typedef __m128i* block128;
    block128 get_receiver_output_128(int i) { return receiverOutputMatrix.squares[i].rows; }
    block128 get_sender_output_128(int choice, int i) { return senderOutputMatrices[choice].squares[i].rows; }

protected:
    void hash_outputs(int nOTs);
};


inline octet* OTExtensionWithMatrix::get_receiver_output(int i)
{
    return (octet*)&receiverOutputMatrix.squares[i/128].rows[i%128];
}

inline octet* OTExtensionWithMatrix::get_sender_output(int choice, int i)
{
    return (octet*)&senderOutputMatrices[choice].squares[i/128].rows[i%128];
}

#endif /* OT_OTEXTENSIONWITHMATRIX_H_ */
