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
    if(count == 0)
    {
        for (int i = 0; i< BUFFER_SIZE; i++)
            new_output[i] = input[i];
            count ++;
    }
    else 
    {

        nlms(past_output,input,new_output);
        
    }
    
    
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
    double dbFactor = 3;
    for (int i =0; i < BUFFER_SIZE; i++)
    {
        m_data[i] = (pow(10, log10(Emag[i]) + dbFactor/20)) * w_data[i];
    }
    ifft(m_data);
    
    
    if (count == 1)
    {
            ofstream outfile;
            outfile.open("Emag.dat", ios::binary| ios::out);
            outfile.write((char*)Emag, BUFFER_SIZE*sizeof(double));
            outfile.close();
    }
    
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
       // readMutex.lock();
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
                //printf("%f, ", f);
             }   
             
             //for (int i = 0 ; i <BUFFER_SIZE; i++)
            // {
                 //printf("%f," float_input[i]);
             //}
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
            float max = -1;
            int shift_max = 0;
            for (int shift = 0; shift < 900; shift +=1)
            {
                for (int i = 0; i < BUFFER_SIZE; i ++)
                    shifted_mask[(i+shift)%BUFFER_SIZE] = 1*float_past_output[i];
                    //shifted_mask[i] = float_past_output[(i+shift)%BUFFER_SIZE];
                float tmp_corr = max_corr(shifted_mask, float_input);
                //printf("%f\n",tmp);
                if (tmp_corr > max)
                {
                    max = tmp_corr;
                    shift_max = shift;
                }
            }
            printf("number of shift is %d\n",shift_max);
            printf("corr is %f\n",max);
            for (int i = 0; i < BUFFER_SIZE; i ++)
                shifted_mask[(i+shift_max)%BUFFER_SIZE] = 1*float_past_output[i];
               //processData(float_input,float_past_output,float_new_output); 
            processData(float_input,shifted_mask,float_new_output,count,w_data);
            //processData(shifted_mask,float_input,float_new_output);
            //writeMutex.lock();
            //processData(pcm_ds_input_buffer, pcm);
            
            //while(1)
            //{
                //if (output1_change)
                //{
                        
                    /*
                    for (int i = 0; i < BUFFER_SIZE * BITS_PER_SAMPLE / 8; i ++)
                    {
                        //pcm_ds_output_buffer[i] = pcm_ds_input_buffer[i];
                        //volatile_output[i] = volatile_input[i];
                        temp_output_buffer[i] = volatile_input[i];
                    }
                    */
                    
                    for( int i =0;i<BUFFER_SIZE;i++)
                    {
                        float f = float_new_output[i];
                        f = f* 32768;
                        if (f > 32767) f = 32767;
                        if (f < -32768) f = -32768;
                        tmp3[i] = (int16_t)f;
                    }
                    
                    
                    if(temp_count == 0)
                    {
                        temp_count ++;
                        ofstream outfile;
                        outfile.open("input.dat", ios::binary| ios::out);
                        outfile.write(temp_output_buffer, BUFFER_SIZE*2);
                        outfile.close();
                    }
                    
                    
                    //for tesing error_t
    
                    float sum = 0;
                    for (int i = 0; i < BUFFER_SIZE; i ++)
                    {
 
                        
                        sum = sum + (float_new_output[i] -float_past_output[i])*(float_new_output[i] -float_past_output[i]);
                        //sum = sum + (float_input[i] -float_past_output[i])*(float_input[i] -float_past_output[i]);
                    }                   
                    printf("%f\n",sum);
                    sum = (sum)/131072;
                    printf("square error is %f\n", sum);
                    
                    //writeMutex.lock();
                   // output1_ready = true;
                    //writeMutex.unlock();
                    //break;
                //}
                
            //}
            printf("output ready\n");
            //writeMutex.unlock();
            /*
            writeMutex.lock();
            if (!output1_ready)
            {
                output1_ready = true;
                output2_ready = false;
            }
            else 
            {
                output1_ready = false;
                output2_ready = true;
            }
            writeMutex.unlock();
            */
            readMutex.lock();
            
            input1_ready = false;
            readMutex.unlock();
        }
        //readMutex.unlock();
        //usleep(DSP_SLEEP);
    }


}
