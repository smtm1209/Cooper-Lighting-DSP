#include <fstream>
#include <mutex>
#include <unistd.h>
#include "constants.h"

using namespace std;
fstream dsp_log = fstream(DSP_LOG_FILENAME, fstream::out);

/**
 * Function for DSP
 * @param input input data
 * @param output output data
 */
void processData(float input[], float output[])
{
    float avg = 0;
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
    {
        avg += input[i];
    }
    avg /= INPUT_BUFFER_SIZE;

    for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++)
    {
        output[i] = avg;
    }
}

/**
 * Function to run DSP thread
 * @param inputBufer the input buffer
 * @param inputBufferIndex address of the input buffer index
 * @param readMutex the input buffer mutex
 * @param outputBuffer the output buffer
 * @param outputBufferIndex address of the output buffer index
 * @param writeMutex the output buffer mutex
 */
void dsp(float inputBuffer[], int &inputBufferIndex, mutex &readMutex, float *outputBuffer, int &outputBufferIndex, mutex &writeMutex)
{
    if (LOGS_ON)
        dsp_log << getTimestamp() << ": Begin Log" << endl;

    float input[INPUT_BUFFER_SIZE];
    while (true)
    {

        // Read Input Buffer
        readMutex.lock();
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Read mutex locked" << endl
                    << getTimestamp() << ": Reading input buffer" << endl;
        for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
        {
            int index = (inputBufferIndex + i) % INPUT_BUFFER_SIZE;
            input[i] = inputBuffer[index];
        }
        readMutex.unlock();
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Read mutex unlocked" << endl;

        // DSP
        float newOutputBuffer[OUTPUT_BUFFER_SIZE];
        if (LOGS_ON)
        {
            dsp_log << getTimestamp() << ": Processing input array: ";
            printArray(input, INPUT_BUFFER_SIZE, dsp_log);
        }
        processData(input, newOutputBuffer);

        if (LOGS_ON)
        {
            dsp_log << getTimestamp() << ": Writing output buffer: ";
            printArray(newOutputBuffer, OUTPUT_BUFFER_SIZE, dsp_log);
        }

        // Write Output Buffer
        writeMutex.lock();
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Write mutex locked" << endl;

        *outputBuffer = *newOutputBuffer;
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Buffers switched" << endl;

        outputBufferIndex = 0;
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Buffer index reset" << endl;

        writeMutex.unlock();
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Write mutex unlocked" << endl;

        usleep(DSP_SLEEP);
    }

    if (LOGS_ON)
        dsp_log << getTimestamp() << ": End Log" << endl;
}