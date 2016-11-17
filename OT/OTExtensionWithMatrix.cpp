// (C) 2016 University of Bristol. See LICENSE.txt

#include "OTExtensionWithMatrix.h"

void OTExtensionWithMatrix::transfer(int nOTs,
        const BitVector& receiverInput)
{
#ifdef OTEXT_TIMER
    timeval totalstartv, totalendv;
    gettimeofday(&totalstartv, NULL);
#endif
    cout << "\tDoing " << nOTs << " extended OTs as " << role_to_str(ot_role) << endl;
    if (nOTs % nbaseOTs != 0)
        throw invalid_length(); //"nOTs must be a multiple of nbaseOTs\n");
    if (nOTs == 0)
        return;
    // add k + s to account for discarding k OTs
    nOTs += 2 * 128;

    int slice = nOTs / nsubloops / 128;
    BitMatrix t1(nOTs), u(nOTs);
    senderOutputMatrices.resize(2, BitMatrix(nOTs));
    // resize to account for extra k OTs that are discarded
    PRNG G;
    G.ReSeed();
    BitVector newReceiverInput(nOTs);
    for (unsigned int i = 0; i < receiverInput.size_bytes(); i++)
    {
        newReceiverInput.set_byte(i, receiverInput.get_byte(i));
    }

    //BitVector newReceiverInput(receiverInput);
    newReceiverInput.resize(nOTs);

    receiverOutputMatrix.resize(nOTs);

    for (int loop = 0; loop < nloops; loop++)
    {
        // randomize last 128 + 128 bits that will be discarded
        for (int i = 0; i < 4; i++)
            newReceiverInput.set_word(nOTs/64 - i, G.get_word());

        // subloop for first part to interleave communication with computation
        for (int start = 0; start < nOTs / 128; start += slice)
        {
            vector<octetStream> os(2);

            BitMatrixSlice receiverOutputSlice(receiverOutputMatrix, start, slice);
            BitMatrixSlice senderOutputSlices[2] = {
                    BitMatrixSlice(senderOutputMatrices[0], start, slice),
                    BitMatrixSlice(senderOutputMatrices[1], start, slice)
            };
            BitMatrixSlice t1Slice(t1, start, slice);
            BitMatrixSlice uSlice(u, start, slice);

            // expand with PRG and create correlation
            if (ot_role & RECEIVER)
            {
                for (int i = 0; i < nbaseOTs; i++)
                {
                    receiverOutputSlice.randomize(i, G_sender[i][0]);
                    t1Slice.randomize(i, G_sender[i][1]);
                }

                t1Slice ^= receiverOutputSlice;
                t1Slice ^= newReceiverInput;
                t1Slice.pack(os[0]);

//                t1 = receiverOutputMatrix;
//                t1 ^= newReceiverInput;
//                receiverOutputMatrix.print_side_by_side(t1);
            }
#ifdef OTEXT_TIMER
            timeval commst1, commst2;
            gettimeofday(&commst1, NULL);
#endif
            // send t0 + t1 + x
            send_if_ot_receiver(player, os, ot_role);

            // sender adjusts using base receiver bits
            if (ot_role & SENDER)
            {
                for (int i = 0; i < nbaseOTs; i++)
                    // randomize base receiver output
                    senderOutputSlices[0].randomize(i, G_receiver[i]);

                // u = t0 + t1 + x
                uSlice.unpack(os[1]);
                senderOutputSlices[0].conditional_xor(baseReceiverInput, u);
            }
#ifdef OTEXT_TIMER
            gettimeofday(&commst2, NULL);
#ifdef VERBOSE
            double commstime = timeval_diff(&commst1, &commst2);
            cout << "\t\tCommunication took time " << commstime/1000000 << endl << flush;
#endif
            times["Communication"] += timeval_diff(&commst1, &commst2);
#endif

            // transpose t0[i] onto receiverOutput and tmp (q[i]) onto senderOutput[i][0]

#ifdef VERBOSE
            cout << "Starting matrix transpose\n" << flush << endl;
#endif
#ifdef OTEXT_TIMER
            timeval transt1, transt2;
            gettimeofday(&transt1, NULL);
#endif
            // transpose in 128-bit chunks
            if (ot_role & RECEIVER)
                receiverOutputSlice.transpose();
            if (ot_role & SENDER)
                senderOutputSlices[0].transpose();

#ifdef OTEXT_TIMER
            gettimeofday(&transt2, NULL);
#ifdef VERBOSE
            double transtime = timeval_diff(&transt1, &transt2);
            cout << "\t\tMatrix transpose took time " << transtime/1000000 << endl << flush;
#endif
            times["Matrix transpose"] += timeval_diff(&transt1, &transt2);
#endif
        }

        // correlation check
        if (!passive_only)
        {
#ifdef OTEXT_TIMER
            timeval startv, endv;
            gettimeofday(&startv, NULL);
#endif
            check_correlation(nOTs, newReceiverInput);
#ifdef OTEXT_TIMER
            gettimeofday(&endv, NULL);
#ifdef VERBOSE
            double elapsed = timeval_diff(&startv, &endv);
            cout << "\t\tTotal correlation check time: " << elapsed/1000000 << endl << flush;
#endif
            times["Total correlation check"] += timeval_diff(&startv, &endv);
#endif
        }

        hash_outputs(nOTs);
#ifdef OTEXT_TIMER
#ifdef VERBOSE
        gettimeofday(&totalendv, NULL);
        double elapsed = timeval_diff(&totalstartv, &totalendv);
        cout << "\t\tTotal thread time: " << elapsed/1000000 << endl << flush;
#endif
#endif
    }

#ifdef OTEXT_TIMER
    gettimeofday(&totalendv, NULL);
    times["Total thread"] +=  timeval_diff(&totalstartv, &totalendv);
#endif

    receiverOutputMatrix.resize(nOTs - 2 * 128);
    senderOutputMatrices[0].resize(nOTs - 2 * 128);
    senderOutputMatrices[1].resize(nOTs - 2 * 128);
}

/*
 * Hash outputs to make into random OT
 */
void OTExtensionWithMatrix::hash_outputs(int nOTs)
{
#ifdef VERBOSE
    cout << "Hashing... " << flush;
#endif
    octetStream os, h_os(HASH_SIZE);
    square128 tmp;
    MMO mmo;
#ifdef OTEXT_TIMER
    timeval startv, endv;
    gettimeofday(&startv, NULL);
#endif

    for (int i = 0; i < nOTs / 128; i++)
    {
        if (ot_role & SENDER)
        {
            tmp = senderOutputMatrices[0].squares[i];
            tmp ^= baseReceiverInput;
            senderOutputMatrices[0].squares[i].hash_row_wise(mmo, senderOutputMatrices[0].squares[i]);
            senderOutputMatrices[1].squares[i].hash_row_wise(mmo, tmp);
        }
        if (ot_role & RECEIVER)
        {
            receiverOutputMatrix.squares[i].hash_row_wise(mmo, receiverOutputMatrix.squares[i]);
        }
    }
#ifdef VERBOSE
    cout << "done.\n";
#endif
#ifdef OTEXT_TIMER
    gettimeofday(&endv, NULL);
#ifdef VERBOSE
    double elapsed = timeval_diff(&startv, &endv);
    cout << "\t\tOT ext hashing took time " << elapsed/1000000 << endl << flush;
#endif
    times["Hashing"] += timeval_diff(&startv, &endv);
#endif
}
