# FPGA to CPU write side channel

In this folder, there are the source codes of both attacks that exploit the side channel on the write transaction.

## Slow Attack

In this subfolder, there are the sources and the data to replicate the same attack proposed for the read channel but using the side channel on the write transaction of the AXI protocol

## Fast Attack

In this subfolder, there are the sources and the data to replicate the optimized version of the attack that uses the write side channel. In particular, this attack only checks one address for each cache line instead of the 8 that the slow version checks. 