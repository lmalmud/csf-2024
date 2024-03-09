#!/usr/local/bin/bash

make
#if it says you do not have permission to run this, run command: chmod +x runTest.sh
echo "256 4 16 write-allocate write-back lru < read01.trace:"

./csim 256 4 16 write-allocate write-back lru < read01.trace
echo "
"

echo "smallest cache
"
./csim 1 1 4 write-allocate write-back lru < read01.trace

echo "direct map cache with 8 blocks of size 16
"
./csim 1 8 16 write-allocate write-back lru < read01.trace
