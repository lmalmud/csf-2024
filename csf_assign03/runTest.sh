#!/usr/local/bin/bash

make
# if it says you do not have permission to run this, run command: chmod +x runTest.sh
# echo "256 4 16 write-allocate write-back lru < read02.trace:"

# ./csim 256 4 16 write-allocate write-back lru < read02.trace
# echo " should have ~400 cycles and 9 load hits and one load miss
# "

# echo "smallest cache
# "
# ./csim 1 1 4 write-allocate write-back lru < read01.trace

# echo "direct map cache with 8 blocks of size 16
# "
# ./csim 8 1 16 write-allocate write-back lru < read01.trace

echo "
assured test case: /csim 256 4 16 no-write-allocate write-through lru

gcc

"
./csim 256 4 16 no-write-allocate write-through lru < gcc.trace
echo "Expected:
Total loads: 318197
Total stores: 197486
Load hits: 311613
Load misses: 6584
Store hits: 164819
Store misses: 32667
Total cycles: 22700397
"
echo "
swim:
"

./csim 256 4 16 no-write-allocate write-through lru < swim.trace
echo "Expected:
Total loads: 220668
Total stores: 82525
Load hits: 218072
Load misses: 2596
Store hits: 58030
Store misses: 24495
Total cycles: 9511568
"

echo "
read01
"

./csim 256 4 16 no-write-allocate write-through lru < read01.trace
echo "Expected:
Total loads: 5
Total stores: 0
Load hits: 2
Load misses: 3
Store hits: 0
Store misses: 0
Total cycles: 1205
"

echo "
read02:
"

./csim 256 4 16 no-write-allocate write-through lru < read02.trace
echo "Expected:
Total loads: 10
Total stores: 0
Load hits: 9
Load misses: 1
Store hits: 0
Store misses: 0
Total cycles: 410
"

echo "
read03:
"

./csim 256 4 16 no-write-allocate write-through lru < read03.trace
echo "Expected:
Total loads: 9
Total stores: 0
Load hits: 5
Load misses: 4
Store hits: 0
Store misses: 0
Total cycles: 1609
"


echo "
write01:
"

./csim 256 4 16 no-write-allocate write-through lru < write01.trace
echo "Expected:
Total loads: 0
Total stores: 5
Load hits: 0
Load misses: 0
Store hits: 0
Store misses: 5
Total cycles: 500
"

echo "
write02:
"

./csim 256 4 16 no-write-allocate write-through lru < write02.trace
echo "Expected:
Total loads: 0
Total stores: 10
Load hits: 0
Load misses: 0
Store hits: 0
Store misses: 10
Total cycles: 1000
"



# echo "second courselore post
# "
# ./csim 8 1 16 write-allocate write-back lru < testMiss.trace
echo "done"