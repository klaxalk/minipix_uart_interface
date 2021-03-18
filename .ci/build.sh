#!/bin/bash
set -e

sudo apt-get -y install cmake build-essential gcc g++

cd software

mkdir build
cd build
cmake ..
make
