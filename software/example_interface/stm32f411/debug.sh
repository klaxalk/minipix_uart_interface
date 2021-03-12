#!/bin/bash

# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

SW_PATH=$MY_PATH
BUILD_PATH=$SW_PATH/build

# run the on-chip-debugger in the background
openocd -f /usr/share/openocd/scripts/board/st_nucleo_f4.cfg &

sleep 2

# start gdb and load our program
gdb-multiarch $BUILD_PATH/main.elf -x $MY_PATH/gdb.conf
