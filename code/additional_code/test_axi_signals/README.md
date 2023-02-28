# Test AXI Signal

In this folder you can find the relevant file to the custom IP developed with HLS to test the handshakes time with the AXI interface

## Goal

The goal is to observe (with an ILA) the handshakes pattern when reading data through the axi (ACP) interface. In particular, the differences in clock cycles indicate a cache hit vs a cache miss.
## Design

![Design](https://c4science.ch/diffusion/12597/browse/master/images/designs/axi_master_reader_HP.png)


## Relevant notes

Be aware that prefetching might occur (it has not been studied yet) thus you have to read address which are apart from each other (pos 0 and pos 512 for example) to be sure to observe real difference. 

