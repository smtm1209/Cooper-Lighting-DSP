#include <mutex>
#include <thread>
#include <vector>
#include "constants.h"
#include "readMicrophone.cpp"
#include "dsp.cpp"
#include "writeSpeaker.cpp"

using namespace std;
fstream buffer_log = fstream(BUFFER_LOG_FILENAME, fstream::out);

/**
 * Main function to start Microphone, DSP, and Speaker threads
 * @param argc unused
 * @param argv unused
 * @return unused
 */
int main(int argc, char *argv[])
{
    if (LOGS_ON)
        buffer_log << getTimestamp() << ": Begin Log" << endl;

    float inputBuffer[INPUT_BUFFER_SIZE];
    int inputBufferIndex = 0;

    float *outputBuffer;
    outputBuffer = new float[OUTPUT_BUFFER_SIZE];
    int outputBufferIndex = 0;

    mutex readMutex;
    mutex writeMutex;
    vector<thread> threads;

    // Thread to parse Microphone input
    threads.push_back(thread(readMicrophone, inputBuffer, ref(inputBufferIndex), ref(readMutex)));
    if (LOGS_ON)
        buffer_log << getTimestamp() << ": Microphone thread added" << endl;

    // Thread for Digital Signal Processing
    threads.push_back(thread(dsp, inputBuffer, ref(inputBufferIndex), ref(readMutex), outputBuffer, ref(outputBufferIndex), ref(writeMutex)));
    if (LOGS_ON)
        buffer_log << getTimestamp() << ": DSP thread added" << endl;

    // Thread to write to Speaker
    threads.push_back(thread(writeSpeaker, outputBuffer, ref(outputBufferIndex), ref(writeMutex)));
    if (LOGS_ON)
        buffer_log << getTimestamp() << ": Speaker thread added" << endl;

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }
    if (LOGS_ON)
        buffer_log << getTimestamp() << ": Threads joined" << endl;

    if (LOGS_ON)
        buffer_log << getTimestamp() << ": End Log" << endl;
}