# Test AXI Reader with Timer

In this folder you can find the relevant file to the custom IP developed with HLS to test the ability of reading from the timer

## Goal

The goal is to observe and time the handshakes pattern when reading data through the axi (ACP) interface. In particular, the differences in clock cycles indicate a cache hit vs a cache miss. The timer is responsile for the timing of the AXI but the processing of the information should be done on the HLS custum IP. So far, the IP is able to read the timer and use its value

## Design

![Design](https://c4science.ch/diffusion/12597/browse/master/images/designs/axi_master_reader_HP.png)


