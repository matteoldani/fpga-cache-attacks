# Memory Tester

In this folder there are all the files used to build the memroy tester. In particular this project did not used the PL part of the board, thus the platform can be build on top of any other platform which include the Zynq 7000 component

## Goal

The goal is to test memory limitation issues arised while building the attack proposed in the papaer. More specifically the board appeared to be limited to 2Kb both for dynamic allocation (heap) and static one (stack). This test confirmed the limitation which was caused by the linker script, which was limiting the size of both heap and stack

