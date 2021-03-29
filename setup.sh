  
#!/bin/bash

export THIS_DIR="$(pwd)"
export PIPE_FILE=$THIS_DIR'/play/audio_pipe'
export PLAY_DIR=$THIS_DIR'/play'
export SCRIPT_DIR="$(pwd)""/scripts"
echo "Script dir here: "$SCRIPT_DIR

if [ ! -f $PLAY_DIR ];then
	mkdir -p $PLAY_DIR
fi

if [ ! -f $PLAY_DIR/.asoundrc ];then
       cp $SCRIPT_DIR/arecord_config $PLAY_DIR/.asoundrc
fi

if [ ! -p $PIPE_FILE ];then
	mkfifo $PIPE_FILE;
fi
