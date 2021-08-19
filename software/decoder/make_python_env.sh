#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

sudo apt-get -y install python3-venv
sudo apt-get -y install python3-pip

python3 -m venv python_env

source ./python_env/bin/activate

pip3 install -r requirements.txt
