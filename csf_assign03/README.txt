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