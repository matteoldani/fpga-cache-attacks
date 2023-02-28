# Axi Master Reader ACP

In this folder you can find the relevant file to the custom IP developed with HLS to test the capabilities of the ACP port

## Goal

The goal is to build an IP which can be controlled by the PS and able to write coherently from the L2 cache of the CPU. The IP is given a value and an address, its duty is to write the value 256 times startig at the given address. The value is passed through a AXI-lite slave interface. The base address is given as offset to the AXI-full Master interface which uses the ACP port to directly access cache

## Design

![Design](https://c4science.ch/diffusion/12597/browse/master/images/designs/axi_master_reader_ACP.png)


## Relevant notes

For the ACP to work, four signals have to be set to 1: AWUSER[0], AWCACHE[1], ARUSER[0] and ARCACHE[1]

