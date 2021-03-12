#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

BUILD_PATH=$MY_PATH/build

# main.elf -> main.bin
arm-none-eabi-objcopy -O binary $BUILD_PATH/main.elf $BUILD_PATH/main.bin

# flash the firmware
st-flash write $BUILD_PATH/main.bin 0x08000000
