#include <fstream>
#include <iostream>
#include <mutex>
#include <unistd.h>
#include "constants.h"

using namespace std;
fstream speaker_log = fstream(SPEAKER_LOG_FILENAME, fstream::out);

/**
 * Function to output data to speaker
 * @param value the value to write to the speaker
 */
void writeToSpeaker(float value)
{
    cout << value << endl; // Replace with write speaker value
}

/**
 * Function for writing to speaker
 * @param outputBuffer the output buffer
 * @param outputBufferIndex address of the output buffer index
 * @param writeMutex the output buffer mutex
 */
void writeSpeaker(float *outputBuffer, int &outputBufferIndex, mutex &writeMutex)
{
    if (LOGS_ON)
        speaker_log << getTimestamp() << ": Begin Log" << endl;

    float outputValue;
    while (true)
    {
        // Read Output Buffer
        writeMutex.lock();
        if (LOGS_ON)
            speaker_log << getTimestamp() << ": Write mutex locked" << endl;

        outputValue = outputBuffer[outputBufferIndex++];
        if (LOGS_ON)
            speaker_log << getTimestamp() << ": Value " << outputValue << " received from outputBuffer at index " << outputBufferIndex - 1 << endl;

        outputBufferIndex = outputBufferIndex % OUTPUT_BUFFER_SIZE;

        writeMutex.unlock();
        if (LOGS_ON)
            speaker_log << getTimestamp() << ": Write mutex unlocked" << endl;

        // Output Sound
        writeToSpeaker(outputValue);
        if (LOGS_ON)
            speaker_log << getTimestamp() << ": Value " << outputValue << " written to speaker" << endl;

        usleep(SPEAKER_SLEEP);
    }

    if (LOGS_ON)
        speaker_log << getTimestamp() << ": End Log" << endl;
}