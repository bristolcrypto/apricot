// (C) 2016 University of Bristol. See LICENSE.txt

#include <fstream>

#include "Test/OutputCheck.h"
#include "OT/BitVector.h"

using namespace std;

int main(int argc, char** argv)
{
    int result = 0;

    for (int receiver = 0; receiver < 2; receiver++)
    {
        BitVector receiverInput, receiverOutput, senderOutput, dummy;


        char filename[1024];
        sprintf(filename, RECEIVER_INPUT, receiver);
        ifstream inf(filename);
        receiverInput.input(inf, false);
        sprintf(filename, RECEIVER_OUTPUT, receiver);
        ifstream receiverOutputFile(filename);
        ifstream senderOutputFile[2];
        for (int i = 0; i < 2; i++)
        {
            sprintf(filename, SENDER_OUTPUT, 1 - receiver, i);
            senderOutputFile[i].open(filename);
        }

        for (int i = 0; i < receiverInput.size(); i++)
        {
            int choice = receiverInput.get_bit(i);
            receiverOutput.input(receiverOutputFile, false);
            senderOutput.input(senderOutputFile[choice], false);
            dummy.input(senderOutputFile[1-choice], false);

            if (!receiverOutput.equals(senderOutput))
            {
                cout << "Mismatch in OT " << i << " with receiver "
                    << receiver << ", choice " << choice << endl;
                cout << "Sender: ";
                senderOutput.output(cout, true);
                cout << endl << "Receiver: ";
                receiverOutput.output(cout, true);
                cout << endl;
                result = 1;
                exit(1);
            }
        }
    }

    return result;
}
