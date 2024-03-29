Brady Bock
Lucy Malmud

Brady worked on...
- designing the structure of the program: creating all files
- implementing the slot, set, and cache classes
- counting the statistics
- debugging the bash file

Lucy worked on...
- handling input
- getter methods for address parts
- managing block evictions, additions, etc
- writing the bash file to design the experiments

***

For our experiment, we ran all possible combinations of sets and block
sizes of powers of 2 in the range [2, 256] and number of num_bytes
in the range [4, 256]. For each of these configurations, we then also
tested all valid replacement and writing policies. We recorded the results
in a spreadsheet, and then sorted by the total number of cycles. We ran all
configurations on swim.trace.

We decided that 524288, as it is about half of one MB of data. We sorted
all of the results collected from the bash script by size, separating all of
the ones that had the desired total size.

We consistently see that write-allocate write-through is the best combination
of both writing. It was also clear that having 256 blocks
per byte resulted in consistently poor performance with various combinations of
number of sets and number of blocks.

Additionally, we see that write-allocate write-back is the best combination of replacement
and writing policies. We consistently saw that 4 bytes per block produced the lowest
total number of cycles, where various combinations of set and block numbers also produced
optimal results.

We note that for a fixed total amount of storage, there the same writing policy combination
could produce the same result whether it was paired with a lru or fifo replacement policy, but
the optimal policy is dependent upon the number of blocks and bytes per block.

All results may be found in CSF_O3_excel.
Below are the best cache configurations, all of which resulted in a cycle count of 2539993.
32	4096	4	write-allocate write-back	lru
64	2048	4	write-allocate write-back	lru
128	1024	4	write-allocate write-back	lru
256	512	4	write-allocate write-back	lru
32	4096	4	write-allocate write-back	fifo
64	2048	4	write-allocate write-back	fifo
128	1024	4	write-allocate write-back	fifo
256	512	4	write-allocate write-back	fifo
Here are the worst cache configurations, all of which resulted in a cycle count of 14831193.
1	2048	256	write-allocate write-through	lru
2	1024	256	write-allocate write-through	lru
4	512	256	write-allocate write-through	lru
8	256	256	write-allocate write-through	lru
16	128	256	write-allocate write-through	lru
32	64	256	write-allocate write-through	lru
64	32	256	write-allocate write-through	lru
128	16	256	write-allocate write-through	lru
256	8	256	write-allocate write-through	lru
1	2048	256	write-allocate write-through	fifo
2	1024	256	write-allocate write-through	fifo
4	512	256	write-allocate write-through	fifo
8	256	256	write-allocate write-through	fifo
16	128	256	write-allocate write-through	fifo
32	64	256	write-allocate write-through	fifo
64	32	256	write-allocate write-through	fifo
128	16	256	write-allocate write-through	fifo
256	8	256	write-allocate write-through	fifo