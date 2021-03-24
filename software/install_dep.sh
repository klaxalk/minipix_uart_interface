#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

cd $MY_PATH

git submodule update --init --recursive

sudo apt-get -y install libopencv-dev tmux tmuxinator socat
