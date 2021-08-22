#!/bin/bash

set -e

sudo apt-get -y update

sudo apt-get -y install cmake build-essential gcc g++ libopencv-dev

cd software

mkdir build
cd build
cmake ..
make
