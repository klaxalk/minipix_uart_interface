#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

cd $MY_PATH

./build/example_oneweb /tmp/ttyS1 921600 1 `pwd`/received_data/data.txt
