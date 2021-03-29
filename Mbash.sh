#!/bin/bash

#export SCRIPT_DIR="$(pwd)""/scripts"
source $SCRIPT_DIR/setup.sh
echo "Script dir: "$SCRIPT_DIR
echo "Pipe file: "$PIPE_FILE

# the following is for short duration testing, duration 5 seconds then play .wav*/

cd $PLAY_DIR
arecord -D dmic_sv -c2 -r 11025 -d 5 -f S32_LE -t raw -V stereo -v $PIPE_FILE 

# the following is bash command to be used on startup*/
# arecord -D dmic_sv -c2 -r 11025 -f S32_LE -t wav -V stereo -v apipe */
