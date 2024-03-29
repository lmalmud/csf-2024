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

The worst configuration (with the highest total cycle count) was:
2 2	256	write-allocate write-back fifo with 810491993 cycles
We noticed that the worst number of sets, blocks, and bytes is certainly 2, 2, and 256.

The best configuration (with the lowest total cycle count) was
256	256	4 write-allocate write-back	fifo with 2539993 cycles
We note that for many block sizes, it was evident that write-allocate write-back is
the optimal combination of writing policies. The choice of replacement policy did
not have as large of an impact on the total number of cycles.