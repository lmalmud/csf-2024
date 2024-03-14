#!/usr/local/bin/bash

make
# if it says you do not have permission to run this, run command: chmod +x runTest.sh
echo "256 4 16 write-allocate write-back lru < read02.trace:"

./csim 256 4 16 write-allocate write-back lru < read02.trace
echo " should have ~400 cycles and 9 load hits and one load miss
"

echo "smallest cache
"
./csim 1 1 4 write-allocate write-back lru < read01.trace

echo "direct map cache with 8 blocks of size 16
"
./csim 8 1 16 write-allocate write-back lru < read01.trace

echo "top courselore post gcc.trace
"
./csim 256 4 16 write-allocate write-back fifo < gcc.trace
echo " Expected:
Total loads: 318197
Total stores: 197486
Load hits: 314171
Load misses: 4026
Store hits: 188047
Store misses: 9439
Total cycles: 9845283
"

# echo "second courselore post
# "
# ./csim 8 1 16 write-allocate write-back lru < testMiss.trace
echo "done"