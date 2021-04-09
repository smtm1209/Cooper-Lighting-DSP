#include <fstream>
#include <iostream>
#include <mutex>
#include <unistd.h>
#include "constants.h"
#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
fstream speaker_log = fstream(SPEAKER_LOG_FILENAME, fstream::out);


/**
 * Function for writing to speaker
 * @param outputBuffer the output buffer
 * @param outputBufferIndex address of the output buffer index
 * @param writeMutex the output buffer mutex
 */
void writeSpeaker(char* pcm_ds_output_buffer, mutex &writeMutex, bool &input1_ready, bool &output1_ready, bool &input1_capture, bool &output2_ready, mutex &readMutex, char* temp_output_buffer)
{
	unsigned int pcm, tmp, dir;
    snd_pcm_state_t state;
    unsigned int rate;
    int outputCount = 0;
	int channels;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
    snd_pcm_uframes_t val;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    char *buff;
    char tmp_data;
    int tempCount = 0;
	//char *buff;

	int buff_size, loops;
    int sample_size = INPUT_BUFFER_SIZE;

    
    rate     = 44100;
    channels = 2;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK,  0) < 0) 
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params,
						SND_PCM_FORMAT_S16_LE) < 0) 
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) 
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
		printf("(mono)\n");
	else if (tmp == 2)
		printf("(stereo)\n");

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);


	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);
    snd_pcm_hw_params_get_buffer_size(params, &val);
    printf("buffer size is %ld\n",val); 
	buff_size = frames * channels * 2 /* 2 -> sample size */;
    printf("buff_size %d\n", buff_size);
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
    int offset = 0;
    printf("hello?\n");

    snd_pcm_wait(pcm_handle,-1);
    //snd_pcm_prepare(pcm_handle);
	//for (loops = (seconds * 1000000) / tmp; loops > 0; loops--) 
    int count = 0;
    while (1) {
            if (outputCount < 5 )
            {
                if (outputCount ==2)
                {
                    readMutex.lock();
                    input1_capture = true;
                    readMutex.unlock();
                }
                writeMutex.lock();
                for (int i = 0; i < buff_size/2; i++)
                {
                    if (i % 2 == 0)
                    {
                        buff[i*2] = pcm_ds_output_buffer[i+offset];
                        buff[i*2+2] = pcm_ds_output_buffer[i+offset];
                    }
                    else
                    {
                        buff[i*2-1] = pcm_ds_output_buffer[i+offset];
                        buff[i*2+1] = pcm_ds_output_buffer[i+offset];
                    }
                }
                writeMutex.unlock();
                offset = offset + buff_size/2;

                if (offset >= BUFFER_SIZE * snd_pcm_format_width(format) / 8) {
                    offset = 0;
                    printf("3 seconds pushed to playback pipe\n");
                    outputCount ++;
                }

                
                pcm = snd_pcm_writei(pcm_handle, (const void*)buff, frames);
                if (pcm == -EPIPE) {
                    printf("XRUN.\n");
                    snd_pcm_prepare(pcm_handle);
                } else if (pcm < 0) {
                    printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
                }
            }
            else 
            {
                tempCount ++;
                outputCount = 0;
                printf("drain start\n");
                snd_pcm_drain(pcm_handle);
                printf("drain complete\n");
                snd_pcm_prepare(pcm_handle);
                for (int i = 0; i < BUFFER_SIZE * BITS_PER_SAMPLE / 8; i ++)
                    {
                        pcm_ds_output_buffer[i] = temp_output_buffer[i];
                    }
            }



	}

	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);

}
