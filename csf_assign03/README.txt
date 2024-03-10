A directly-mapped cache (1 set) with 4 blocks per set where each block has 4 bytes
// sets blocks bytesPerBlock
./csim 1 4 32 write-allocate write-through lru

0x12345678 example
offset is "12" (first 16 bits)
index is "3456" (middle 26 bits)
tag is "8" (last 8 bits)
./csim 1 1048576 256 write-allocate write-through lru