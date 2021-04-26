#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "util.cpp"

#define BROKER_IP "192.168.137.174"
#define PORT 1883

#define INPUT_BUFFER_SIZE 131072
#define OUTPUT_BUFFER_SIZE 131072
#define BUFFER_SIZE 131072
#define BITS_PER_SAMPLE 16
#define FILTER_LENGTH 64
#define INPUT_CHANNEL 2
#define OUTPUT_CHANNEL 2
#define SAMPLE_RATE 44100
#define CHANNELS 2

#define MIC_SLEEP 10000
#define DSP_SLEEP 10000
#define SPEAKER_SLEEP 1000

#define LOGS_ON true
#define BUFFER_LOG_FILENAME "bufferlog.txt"
#define MICROPHONE_LOG_FILENAME "miclog.txt"
#define DSP_LOG_FILENAME "dsplog.txt"
#define SPEAKER_LOG_FILENAME "speakerlog.txt"

#define PCM_DEVICE "hw:2"
#endif
