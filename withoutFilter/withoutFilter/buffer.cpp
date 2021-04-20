#include <mutex>
#include <thread>
#include <fstream>
#include <random>
#include <vector>
#include <alsa/asoundlib.h>
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
    mutex readMutex;
    mutex writeMutex;
    bool input1_capture = true;
    bool input1_ready = false;
    bool output1_change = false;
    bool output1_ready =false;
    bool output2_ready = false;
    vector<thread> threads;

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0,1.0);
    float* whiteNoise = (float*) malloc(sizeof(float)*BUFFER_SIZE);
    for (int i=0; i< BUFFER_SIZE; ++i) {
        whiteNoise[i] = distribution(generator);
    }
    
    float max = 0;
    for (int i = 0; i < BUFFER_SIZE; i ++)
    {
        if (max < abs(whiteNoise[i]))
            max = abs(whiteNoise[i]);
    }
    for (int i =0 ; i<BUFFER_SIZE; i++)
    {
        whiteNoise[i] = 0.1*whiteNoise[i]/max;
    }
    
    char* pcm_ds_input_buffer = (char*)malloc(BUFFER_SIZE * BITS_PER_SAMPLE / 8);
    char* pcm_ds_output_buffer = (char*)malloc(BUFFER_SIZE * BITS_PER_SAMPLE / 8);
    char* temp_output_buffer = (char*)malloc(BUFFER_SIZE * BITS_PER_SAMPLE / 8) ;
       
    for (int i = 0; i < BUFFER_SIZE*BITS_PER_SAMPLE / 8; i++)
    {
        pcm_ds_output_buffer[i] = 0;
    }
    
    // Thread to parse Microphone input
    threads.push_back(thread(readMicrophone, pcm_ds_input_buffer, ref(readMutex), ref(input1_ready), ref(output1_ready), ref(input1_capture)));

    // Thread for Digital Signal Processing
    threads.push_back(thread(dsp, pcm_ds_input_buffer, ref(readMutex), pcm_ds_output_buffer, ref(writeMutex), ref(input1_ready), ref(output1_ready), ref(output2_ready), ref(output1_change), temp_output_buffer, whiteNoise));

    // Thread to write to Speaker
    //threads.push_back(thread(writeSpeaker, pcm_ds_output_buffer, ref(writeMutex), ref(input1_ready), ref(output1_ready), ref(input1_capture), ref(output2_ready), ref(readMutex), ref(output1_change), temp_output_buffer));
    threads.push_back(thread(writeSpeaker, pcm_ds_output_buffer, ref(writeMutex), ref(input1_ready), ref(output1_ready), ref(input1_capture), ref(output2_ready), ref(readMutex) , temp_output_buffer ));

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }
}
