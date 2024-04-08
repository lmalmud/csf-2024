CONTRIBUTIONS
Brady Bock
- Debugged forking/creating child processes
- Handled file opening/close
Lucy Malmud
- Handled memory mapping & wrote initial parrelizaiton code
- Ran the experiment & wrote report

REPORT
Noting that the sys time is how much is spent in the OS kernel and the user time is how
much is actually spent in the process, we found that higher thresholds have a larger user
time and a smaller system time. This is due to the fact that there are fewer child
processes being spawned and the sequential algorithm for sorting is being called on larger datasets.
For lower thresholds, the sequential sort acts on smaller datasets, so more child processes are spawned,
increasing the system time because there is more overhead in the OS Kernel switching from process to process.

We note that the optimal threshold value was at 262144. Before this value, there was more time
spent in the userspace, and afterwards there was too much overhead for the OS kernel and the majority
of time was spent switching between processes.

Test run with threshold 2097152

real    0m0.375s
user    0m0.365s
sys     0m0.008s
Test run with threshold 1048576

real    0m0.226s
user    0m0.378s
sys     0m0.027s
Test run with threshold 524288

real    0m0.154s
user    0m0.413s
sys     0m0.029s
Test run with threshold 262144

real    0m0.136s
user    0m0.547s
sys     0m0.048s
Test run with threshold 131072

real    0m0.140s
user    0m0.539s
sys     0m0.063s
Test run with threshold 65536

real    0m0.143s
user    0m0.562s
sys     0m0.075s
Test run with threshold 32768

real    0m0.148s
user    0m0.590s
sys     0m0.105s
Test run with threshold 16384

real    0m0.158s
user    0m0.604s
sys     0m0.156s