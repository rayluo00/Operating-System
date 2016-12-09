Weiming Raymond Luo
CSCI 460 : Operating Systems
Assignment 3 - Page Replacement Algorithms
November 21, 2016

====================================== DESCRIPTION  ====================================

This assignment provides two implementations of page replacement algorithms using an
inverted page table. One page algorithm is the Least Recently Used (LRU), Enhanced
Second-Chance algoirthm and the other is the Least Frequently Used (LFU) algorithm.
The LRU replaces pages by checking for clean pages first to replaces or if the pages
had been referenced or modified. While the LFU keeps a counter of the times when a
page has been referenced, with the least referenced page being replaced. After a
specfic reference count to the pages, all reference counts are halved.

From my test after conducting five trails for each algorithm, my data (See below) shows
that the LRU algoirthm proved to be more efficient with page replacement. On average,
the LRU had reading page faults of 372.6, writing page faults of 31.4, and total page
faults of 404. While the LFU had average reading page faults of 592.4, writing page
faults of 141.6, and total page faults of 734. After conducting the test, the LFU had
303 more total page faults compared to LRU, which is 81.68% more page faults on average.

========================================= DATA =========================================
LRU

TRAIL 1
Reading page faults             : 345
Writing and reading page faults : 32
Total page faults               : 377
-------------------------------------
TRAIL 2
Reading page faults             : 387
Writing and reading page faults : 33
Total page faults               : 420
-------------------------------------
TRAIL 3
Reading page faults             : 423
Writing and reading page faults : 25
Total page faults               : 448
-------------------------------------
TRAIL 4
Reading page faults             : 375
Writing and reading page faults : 35
Total page faults               : 410
-------------------------------------
TRAIL 5
Reading page faults             : 333
Writing and reading page faults : 32
Total page faults               : 365
--------------------------------------

LFU

TRAIL 1
Reading page faults             : 571
Writing and reading page faults : 128
Total page faults               : 699
-------------------------------------
TRAIL 2
Reading page faults             : 622
Writing and reading page faults : 140
Total page faults               : 762
-------------------------------------
TRAIL 3
Reading page faults             : 590
Writing and reading page faults : 144
Total page faults               : 734
-------------------------------------
TRAIL 4
Reading page faults             : 592
Writing and reading page faults : 159
Total page faults               : 751
-------------------------------------
TRAIL 5
Reading page faults             : 587
Writing and reading page faults : 137
Total page faults               : 724
-------------------------------------
