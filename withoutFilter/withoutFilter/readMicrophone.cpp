#include <fstream>
#include <mutex>
#include <unistd.h>
#include "constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

using namespace std;


/**
 * Function for parsing microphone input
 * @param inputBufer the input buffer
 * @param inputBufferIndex address of the input buffer index
 * @param readMutex the input buffer mutex
 */
void readMicrophone(char* pcm_ds_input_buffer, mutex &readMutex, bool &input1_ready, bool &output1_ready, bool &input1_capture)
{   
    int i;
    int err;
    
    char *buffer;
    char *DownSizedBuffer;

    char* pcm_input_buffer = (char*)malloc(BUFFER_SIZE * BITS_PER_SAMPLE / 8 * INPUT_CHANNEL);

    unsigned int rate = SAMPLE_RATE;
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;


    // open audio interface for mic input
    if ((err = snd_pcm_open (&capture_handle, "hw:2", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf (stderr, "cannot open audio device %s (%s)\n", 
                "hw:0",
                snd_strerror (err));
        exit (1);
    }
    fprintf(stdout, "audio interface opened\n");
            
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params allocated\n");
                    
    if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params initialized\n");
        
    if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params access setted\n");
        
    if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params format setted\n");
        
    if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
                snd_strerror (err));
        exit (1);
    }
        
    fprintf(stdout, "hw_params rate setted\n");

    if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "hw_params channels setted\n");
        
    if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
                snd_strerror (err));
        exit (1);
    }
    snd_pcm_uframes_t val;
    snd_pcm_uframes_t frames;
    snd_pcm_hw_params_get_buffer_size(hw_params, &val);
    snd_pcm_hw_params_get_period_size(hw_params, &frames, 0);
    int buff_size = frames * 2 * 2; //frames * channels * snd_pcm_format_width(format) / 8
    char* buff;
    buff = (char*) malloc(buff_size);
    printf("buffer size input is %ld\n",val); 
    printf("frame size per period is %ld\n", frames);

    fprintf(stdout, "hw_params setted\n");
        
    snd_pcm_hw_params_free (hw_params);

    fprintf(stdout, "hw_params freed\n");
        
    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror (err));
        exit (1);
    }

    fprintf(stdout, "audio interface prepared\n");
    buffer = (char*) malloc(BUFFER_SIZE * snd_pcm_format_width(format) / 8 * 2);
    int total = BUFFER_SIZE * snd_pcm_format_width(format) / 8 * 2;
    for (int i = 0; i < BUFFER_SIZE * BITS_PER_SAMPLE / 8 *2; i++)
    {
        buffer[i] = 0;
    }
    DownSizedBuffer = (char*) malloc(BUFFER_SIZE * snd_pcm_format_width(format) / 8 );
    int offset = 0;
    fprintf(stdout, "buffer allocated\n");

    int inputCount = 0;
    int a = 0;
    while(1)
    {
        //readMutex.lock();
        if (a == 0)
            snd_pcm_prepare(capture_handle);
        if (input1_capture)
        {
            a ++;
            err = snd_pcm_readi (capture_handle, buff, frames);
            if (err == -EPIPE) {
                printf("read from audio interface failed \n");
                printf("read %d bytes\n",err);
                snd_pcm_prepare(capture_handle);

            } else if (err < 0) {
                    printf("ERROR. Can't read from PCM device. %s\n", snd_strerror(err));
            }

            for (int i = 0; i < buff_size; i++)
            {
                buffer[i+offset] = buff[i];
            }
            offset = offset + buff_size;

            if (offset >= total) {
                    
                    offset = 0;
                    inputCount ++;
            }

            if (inputCount == 1)
            {
                int count = 0;
                a = 0;
                for (int i = 0; i < BUFFER_SIZE * snd_pcm_format_width(format) / 8 * 2; i ++)
                {
                    if ((i/2)%2 == 0) {
                        pcm_ds_input_buffer[count] = buffer[i];
                        count ++;
                    }
                }
                inputCount = 0;
                printf("read done\n");
                readMutex.lock();
                input1_capture = false;
                input1_ready = true;
                readMutex.unlock();
            }
        }

    }
}
