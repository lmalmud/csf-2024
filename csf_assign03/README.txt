// sets blocks bytesPerBlock

0x12345678 example
TAG INDEX OFFSET
offset is "78" (last 16 bits)
index is "23456" (middle 26 bits)
tag is "1" (first 8 bits)
./csim 1 1048576 256 write-allocate write-through lru

The offset says how many bytes are in a block.
Why do we care about it?
I think this would be relevant if we actually cared about the values of the data.
But we don't.

Direct Mapped Cache with write-through and no-write-allocate
There is one set with one block that can only hold four bits.
Since the number of index bits is log2(blocks), there are no index bits.
Thus, the address is just TAG OFFSET.
Here, we have 2 offset bits and 30 tag bits.
./csim 1 1 4 no-write-allocate write-through lru
l 0x5 1     tag is 1 and offset is 1, should be a load miss
l 0x5 1     tag is 1 and offset is 1, should be a load hit
l 0x9 1     tag is 2 and offset is 1, should be a load miss and EVICT the value present

for provided test files...
./csim 256 4 16 write-allocate write-back lru < write01.trace