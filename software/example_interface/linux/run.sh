#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

cd $MY_PATH

./build/example_interface /tmp/ttyS1 921600 1 /tmp/ttyS2 921600 1
