#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

socat -d -d PTY,link=/tmp/ttyS0,rawer,echo=0 PTY,link=/tmp/ttyS1,rawer,echo=0 &
socat -d -d PTY,link=/tmp/ttyS2,rawer,echo=0 PTY,link=/tmp/ttyS3,rawer,echo=0 &

sleep 1

cd $MY_PATH

./build/minipix_dummy
