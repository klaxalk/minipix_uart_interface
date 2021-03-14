#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

socat -d -d PTY,raw,echo=0,flusho=1,ixoff=0,ixon=0,link=/tmp/ttyS0 PTY,raw,echo=0,flusho=1,ixoff=0,ixon=0,link=/tmp/ttyS1 &

sleep 1

cd $MY_PATH

# ./build/minipix_dummy
cat /tmp/ttyS0
