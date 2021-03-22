#!/bin/bash
set -e

sudo apt-get -y install cmake build-essential gcc g++

sudo apt-get -y install cmake libopencv-dev

cd software

mkdir build
cd build
cmake ..
make
