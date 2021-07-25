#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

cd $MY_PATH

./build/minipix_dummy /tmp/ttyS0 921600 1 ./../../vzlusat1-timepix-data/data/raw/above_europe
