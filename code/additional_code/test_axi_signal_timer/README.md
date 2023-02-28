# Test AXI Signal Timer

In this folder you can find the relevant file to the custom IP developed with HLS to test the handshakes time with the AXI interface and time it

## Goal

The goal is to observe and time (with an ILA) the handshakes pattern when reading data through the axi (ACP) interface. In particular, the differences in clock cycles indicate a cache hit vs a cache miss. 

## Design

![Design](https://c4science.ch/diffusion/12597/browse/master/images/designs/axi_master_reader_HP.png)


