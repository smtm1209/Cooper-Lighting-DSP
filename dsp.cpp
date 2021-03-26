#include <fstream>
#include <mutex>
#include <unistd.h>
#include <math.h> 
#include "constants.h"
#include "dspUtil.cpp"
#include <random>

using namespace std;
fstream dsp_log = fstream(DSP_LOG_FILENAME, fstream::out);

/**
 * Function for DSP
 * @param input input data
 * @param output output data
 */
void processData(float input[INPUT_BUFFER_SIZE], float output[OUTPUT_BUFFER_SIZE], float *outputBuffer, float whiteNoise[INPUT_BUFFER_SIZE])
{
    float e[INPUT_BUFFER_SIZE]; //estimated noise
    bool allZero = true;
    //check if current mask is all zeros (which will be the case during intial setup)
    for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++)
    {
        if (outputBuffer[i] == 0.0)
            continue;
        else
        {
            allZero = false;
            break;
        }
    }

    if (allZero)
    {
        for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
            e[i] = input[i];
    }
    else 
    {
        nlms(outputBuffer, input, e);
    }

    // FFT of e 
    Complex e_complex[INPUT_BUFFER_SIZE];
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
    {
        e_complex[i] = (double)e[i];
    }
    CArray e_data(e_complex, INPUT_BUFFER_SIZE);
    fft(e_data);
    double Emag[INPUT_BUFFER_SIZE];
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
        Emag[i] = std::abs(e_data[i]);

    //FFT of white noise
    Complex w_complex[INPUT_BUFFER_SIZE];
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
        w_complex[i] = (double)whiteNoise[i];
    CArray w_data(w_complex, INPUT_BUFFER_SIZE);
    fft(w_data);
    for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
        w_data[i] = w_data[i]/std::abs(w_data[i]);

    //create a sound mask
    double dbFactor = 3.0;
    //double M[OUTPUT_BUFFER_SIZE];
    Complex m_complex[OUTPUT_BUFFER_SIZE];
    CArray m_data(m_complex, OUTPUT_BUFFER_SIZE);
    //Mdb
    for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++)
    {
        //M[i] = (float)20*log10(Emag[i]) + dbFactor;
        m_data = (pow(10, log10(Emag[i]) + dbFactor/20))*w_data[i];
    }
    ifft(m_data);
    
    for(int i = 0; i < OUTPUT_BUFFER_SIZE; i++)
    {
        output[i] = std::real(m_data[i]);
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
    //generate white noise
    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0,1.0);
    float whiteNoise[INPUT_BUFFER_SIZE];
    for (int i=0; i< INPUT_BUFFER_SIZE; ++i) {
        whiteNoise[i] = distribution(generator);
    }
    
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
        processData(input, newOutputBuffer, outputBuffer, whiteNoise);

        if (LOGS_ON)
        {
            dsp_log << getTimestamp() << ": Writing output buffer: ";
            printArray(newOutputBuffer, OUTPUT_BUFFER_SIZE, dsp_log);
        }

        // Write Output Buffer
        writeMutex.lock();
        if (LOGS_ON)
            dsp_log << getTimestamp() << ": Write mutex locked" << endl;

        for (int i = 0; i < OUTPUT_BUFFER_SIZE; i++)
            outputBuffer[i] = newOutputBuffer[i];
        
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
