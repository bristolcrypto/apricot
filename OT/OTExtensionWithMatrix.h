// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef OT_OTEXTENSIONWITHMATRIX_H_
#define OT_OTEXTENSIONWITHMATRIX_H_

#include "OTExtension.h"
#include "BitMatrix.h"

class OTExtensionWithMatrix : public OTExtension
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
    : OTExtension(nbaseOTs, baseLength, nloops, nsubloops, player, baseReceiverInput,
            baseSenderInput, baseReceiverOutput, role, passive) {}

    void transfer(int nOTs, const BitVector& receiverInput);

protected:
    void hash_outputs(int nOTs);
    octet* get_receiver_output(int i);
    octet* get_sender_output(int choice, int i);
};

#endif /* OT_OTEXTENSIONWITHMATRIX_H_ */
