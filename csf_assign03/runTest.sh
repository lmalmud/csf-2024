#!/usr/local/bin/bash

make
#if you do not have permission ot run this, run command: chmod +x runTest.sh
echo "256 4 16 write-allocate write-back lru < read01.trace:"

./csim 256 4 16 write-allocate write-back lru < read01.trace
echo "
"