#include <fstream>
#include <mutex>
#include <unistd.h>
#include <random>
#include "dspUtil.cpp"
#include "constants.h"

using namespace std;
fstream dsp_log = fstream(DSP_LOG_FILENAME, fstream::out);

/**
 * Function for DSP
 * @param input input data
 * @param output new output data
 * @param outputBuffer current output data
 */


void processData(float* input, float*past_output, float*new_output, int &count, CArray w_data)
{
    //if(count == 0)
    //{
    for (int i = 0; i< BUFFER_SIZE; i++)
        new_output[i] = input[i];
     //       count ++;
   // }
    
    //else 
    //{

    //    nlms(past_output,input,new_output);
        
    //}
    
    
    //create sound mask
    Complex e_complex[BUFFER_SIZE];
    for (int i =0; i < BUFFER_SIZE; i++)
    {
        e_complex[i] = (double)new_output[i];
    }
    CArray e_data(e_complex, BUFFER_SIZE);
    fft(e_data);
    double Emag[BUFFER_SIZE];
    for (int i =0; i <BUFFER_SIZE; i++)
        Emag[i] = std::abs(e_data[i]);
        
    Complex m_complex[BUFFER_SIZE];
    CArray m_data(m_complex, BUFFER_SIZE);
    double dbFactor = +3.0;
    for (int i =0; i < BUFFER_SIZE; i++)
    {
        m_data[i] = (pow(10, log10(Emag[i]) + dbFactor/20)) * w_data[i];
    }
    ifft(m_data);
    
    for (int i =0; i < BUFFER_SIZE; i ++)
    {
        new_output[i] = std::real(m_data[i]);
    }
    
    printf("processed data\n");
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
void dsp(char* pcm_ds_input_buffer, mutex &readMutex, char* pcm_ds_output_buffer, mutex &writeMutex, bool &input1_ready, bool &output1_ready, bool &output2_ready, bool &output1_change, char* temp_output_buffer, float* whitenoise)
{
    char* volatile_output = pcm_ds_output_buffer;
    char* volatile_input = pcm_ds_input_buffer;
    float* float_input = (float*)malloc(BUFFER_SIZE*sizeof(float));
    float* float_past_output = (float*)malloc(BUFFER_SIZE*sizeof(float));
    float* float_new_output = (float*)malloc(BUFFER_SIZE*sizeof(float));
    float* e = (float*) malloc(BUFFER_SIZE*sizeof(float));
    float* shifted_mask = (float*) malloc(BUFFER_SIZE*sizeof(float));
    int16_t* tmp = (int16_t*) pcm_ds_input_buffer;
    int16_t* tmp2 = (int16_t*) pcm_ds_output_buffer;
    int16_t* tmp3 = (int16_t*) temp_output_buffer;
    int count = 0;
    bool yy = true;
    int temp_count = 0;
    
    Complex w_complex[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++)
        w_complex[i] = (double)whitenoise[i];
    CArray w_data(w_complex,BUFFER_SIZE);
    fft(w_data);
    for (int i = 0; i < BUFFER_SIZE; i ++)
        w_data[i] = w_data[i]/std::abs(w_data[i]);
        
    while (1)
    {
        
         // Read Input Buffer 
        if (input1_ready)
        {
            
            //convert input to float array
            for (int i =0; i < BUFFER_SIZE; i++)
            {
                float f;
                f = ((float) tmp[i])/ (float) 32768;
                if ( f> 1) f = 1;
                if (f < -1) f = -1;
                float_input[i] = f;
             }   

            //convert past output to floatarry
            for (int i = 0; i < BUFFER_SIZE; i ++)
            {
                float f;
                f = ((float) tmp2[i])/ (float)32768;
                if ( f> 1) f = 1;
                if (f < -1) f = -1;
                float_past_output[i] = f;
                //printf("%f, ", f);
            }

            processData(float_input,float_past_output,float_new_output,count,w_data);
            //processData(float_input,float_past_output,float_new_output,count,w_data);
                    
            for( int i =0;i<BUFFER_SIZE;i++)
            {
                float f = float_new_output[i];
                f = f* 32768;
                if (f > 32767) f = 32767;
                if (f < -32768) f = -32768;
                tmp2[i] = (int16_t)f;
            }
            
            
            
            
            printf("output ready\n");
        
            readMutex.lock();
            output1_ready = true;
            input1_ready = false;
            readMutex.unlock();
        }

    }


}
