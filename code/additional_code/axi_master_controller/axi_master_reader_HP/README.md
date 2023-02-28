# Axi Master Reader HP

In this folder you can find the relevant file to the custom IP developed with HLS to test the capabilities of the HP port

## Goal

The goal is to build an IP which can be controlled by the PS and able to write to memory thansk to the HP port. The IP is given a value and an address, its duty is to write the value 256 times startig at the given address. The value is passed through a AXI-lite slave interface. The base address is given as offset to the AXI-full Master interface which uses the HP port to directly access memory

## Design

![Design](https://c4science.ch/diffusion/12597/browse/master/images/designs/axi_master_reader_HP.png)


## Relevant notes

For the design to work, the Cache should be either disabled or correctly invalidated at the PS side. Indeed, the HP port does not act chorently with the CPU caches.

