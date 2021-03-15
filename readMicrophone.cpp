#include <fstream>
#include <mutex>
#include <unistd.h>
#include "constants.h"

using namespace std;
fstream mic_log = fstream(MICROPHONE_LOG_FILENAME, fstream::out);

int x = 0; // Used for fake microphone input generation
/**
 * Function to parse Microphone input
 * @return the parsed input
 */
float parseMicrophoneInput()
{
    return x++; // Replace with read microphone value
}

/**
 * Function for parsing microphone input
 * @param inputBufer the input buffer
 * @param inputBufferIndex address of the input buffer index
 * @param readMutex the input buffer mutex
 */
void readMicrophone(float inputBuffer[], int &inputBufferIndex, mutex &readMutex)
{
    if (LOGS_ON)
        mic_log << getTimestamp() << ": Begin Log" << endl;

    while (true)
    {
        // Write Input Buffer
        readMutex.lock();
        if (LOGS_ON)
            mic_log << getTimestamp() << ": Read mutex locked" << endl
                    << getTimestamp() << ": Parsing microphone input" << endl;

        float parsedInput = parseMicrophoneInput();
        if (LOGS_ON)
            mic_log << getTimestamp() << ": Writing value " << parsedInput << " to input buffer at index " << inputBufferIndex << endl;
        inputBuffer[inputBufferIndex++] = parsedInput;
        inputBufferIndex = inputBufferIndex % INPUT_BUFFER_SIZE;

        readMutex.unlock();
        if (LOGS_ON)
            mic_log << getTimestamp() << ": Read mutex unlocked" << endl;

        usleep(MIC_SLEEP);
    }

    if (LOGS_ON)
        mic_log << getTimestamp() << ": End Log" << endl;
}