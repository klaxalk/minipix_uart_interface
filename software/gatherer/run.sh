# get the path to this script
MY_PATH=`dirname "$0"`
MY_PATH=`( cd "$MY_PATH" && pwd )`

cd $MY_PATH

# gatherer <serial port> <baud rate> <serial is virtual> <output data file>
./build/gatherer /tmp/ttyS3 921600 1 `pwd`/received_data/data.txt
