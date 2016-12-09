Weiming Raymond Luo
CSCI 460 : Operating Systems
Assignment 2 - 4BSD and ULE schedulers

NOTE : Both the 4BSD and ULE schedulers use the fmin and fmax to get the min and max
       when obtaining the time in msec. Compiling the programs uses the "-std=99"
       flag. A Makefile is also provided to compile the 4bsd and ule program.

This assignment is to implement a basic version of the 4BSD and ULE scheduler
algorithms. The programs will run 1000 rounds with a timeslice of 100
milliseconds. After conducting five trails for each scheduler, I have found that
the 4BSD scheduler performed much fast on average compared to the ULE scheduler.
The 4BSD scheduler has an average of 145.0309896 msec in the ready queue, 31.823275 
msec waiting for I/O, and 18490.1273926 msec for the overhead. While the ULE algorithm 
has an average of 231.0425068 msec in the ready queue, 153.7157018 msec waiting for 
I/O and 18490.1273926 msec for the overhead.

=================================================================================
4BSD DATA

============================================
TRAIL 1
Process spent in ready queue
Minimum : 0.000000
Maximum : 3847.795166
Average : 147.274194

Process spent between I/O and next dispatch
Minimum : 0.000977
Maximum : 1390.374268
Average : 37.835263

Proportion of time spent on overheads
Overhead : 17648.322998
============================================
TRAIL 2
Process spent in ready queue
Minimum : 0.000000
Maximum : 3382.822021
Average : 132.069456

Process spent between I/O and next dispatch
Minimum : 0.001709
Maximum : 664.178955
Average : 20.171740

Proportion of time spent on overheads
Overhead : 17645.119873
============================================
TRAIL 3
Process spent in ready queue
Minimum : 0.000000
Maximum : 3814.333008
Average : 162.742376

Process spent between I/O and next dispatch
Minimum : 0.001709
Maximum : 682.885010
Average : 36.669098

Proportion of time spent on overheads
Overhead : 16749.880371
============================================
TRAIL 4
Process spent in ready queue
Minimum : 0.000000
Maximum : 3846.837891
Average : 134.915563

Process spent between I/O and next dispatch
Minimum : 0.001953
Maximum : 1485.414062
Average : 37.812005

Proportion of time spent on overheads
Overhead : 17529.447998
============================================
TRAIL 5
Process spent in ready queue
Minimum : 0.000000
Maximum : 4371.377930
Average : 148.133359

Process spent between I/O and next dispatch
Minimum : 0.000977
Maximum : 1135.501953
Average : 26.688609

Proportion of time spent on overheads
Overhead : 18089.201904

=================================================================================
ULE DATA

============================================
TRAIL 1
Process spent in ready queue
Minimum : 0.002197
Maximum : 4750.553711
Average : 236.481260

Process spent between I/O and next dispatch
Minimum : 0.016846
Maximum : 523.911865
Average : 156.487937

Proportion of time spent on overheads
Overhead : 18519.736084
============================================
TRAIL 2
Process spent in ready queue
Minimum : 0.001709
Maximum : 2600.420898
Average : 229.933175

Process spent between I/O and next dispatch
Minimum : 0.015137
Maximum : 483.665039
Average : 141.251379

Proportion of time spent on overheads
Overhead : 18406.114990
============================================
TRAIL 3
Process spent in ready queue
Minimum : 0.002686
Maximum : 5079.200195
Average : 226.658119

Process spent between I/O and next dispatch
Minimum : 0.014160
Maximum : 402.961914
Average : 153.297623

Proportion of time spent on overheads
Overhead : 18789.120117
============================================
TRAIL 4
Process spent in ready queue
Minimum : 0.002930
Maximum : 3723.866699
Average : 229.307634

Process spent between I/O and next dispatch
Minimum : 0.014893
Maximum : 433.320801
Average : 149.626346

Proportion of time spent on overheads
Overhead : 18174.108887
============================================
TRAIL 5
Process spent in ready queue
Minimum : 0.002686
Maximum : 4581.869873
Average : 232.472346

Process spent between I/O and next dispatch
Minimum : 0.019775
Maximum : 433.164795
Average : 167.915227

Proportion of time spent on overheads
Overhead : 18588.556885
============================================
