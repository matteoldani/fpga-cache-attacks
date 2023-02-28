# Cache-Based Side-Channel Attacks in FPGA SoCs

This repository contains the code and all the data collected during my semester project done with the [PARSA](https://parsa.epfl.ch/) laboratory at [EPFL](https://www.epfl.ch/en/) during the Fall 2022 semester.

The project was done under the supervision of:
- Dina Mahmoud: dina.mahmoud@epfl.ch
- Dr. Mirjana Stojilovic: mirjana.stojilovic@epfl.ch

## Goal

We explored the possibility of cache-based attacks on heterogeneous platforms such as the AMD-Xilinx Zynq 7000 which combines a field-programmable gate array (FPGA) with an ARM central processing unit (CPU). We built a partial key recovery attack on the advanced encryption standard (AES), which reduces the key search space from 128 to 48 bits, considering two threat models: FPGA to CPU and CPU to FPGA. In the first threat model, we assess the work done by Bossuet et al. ([reference paper](https://www.mdpi.com/2076-3417/11/14/6662/htm)), and we explore a new side channel, showing that a write transaction can also be used to detect timing differences between data inside and outside of the L1 cache. Finally, the attack is tested in the opposite direction, from the CPU to the FPGA, and the results are found to be consistent with the expectations. Our work thoroughly tests the advanced extensible interface (AXI) protocol and analyzes the five channels to identify timing differences. We propose further work to lower the minimum clock speed that can still detect a timing difference.


## Code

Inside the code folder, there is the code for the three main attacks done during the semester. In addition, under `additional_code` there are other smaller projects used for testing and prototyping or as proof of concept. The AES T-Table implementation used in the attacks is in the `aes.h` file.

Generally, every attack folder is divided in:
- Controller: here there is the code running on the ARM CPU controlling the custom IP on the FPGA side
- Data: here there are files with the data used to draw plots and the relevant plots themselves
- hls: here there is the code used in Vitis HLS to synthesize the custom IP starting from C code
- vivado_scripts: here there are the scripts exported from Vivado. Those are useful to recreate the `block design` used during the specific attack

In addition, there are the components used in conjunction with the Attacker IP, in particular, the timer and the mux to implement the evicter. The exported hardware with the bitstream included is also in the folder. 

`heatmap.py` is the script used to build the plots showing the attack. It takes in input the `.out` file that contains the output from the execution of the controller code.


### FPGA to CPU read attack

Inside this folder, there is the implementation of the reference paper. This implementation uses the side channel on the AXI read transaction to build the key recovery attack. Inside the controller folder, there are: 
- `data_collection_paper.c`: this file contains the code used to collect the data as proposed by the reference paper. The associated hls source is `ip_data_collection.cpp`
- `paper_attack.c`: this is the code for the real partial key recovery attack presented in the reference paper. The associated hls source is `ip_paper_attack.cpp`

### FPGA to CPU write attack 

Inside this folder, there is the code showing the new side channel found. The folder contains two subfolders: the normal attack and the optimized version of it. More information is available in the report. 

### CPU to FPGA attack

Inside this folder, there is the code showing the attack in the reversed threat model, as explained in the report. The timer used is inside the controller. 


## Images

In this folder, there are images showing some relevant results achieved during the development. In the `normal write` folder, there are screenshots showing the measurements done while proving that a normal write from the FPGA was able to invalidate data present in the L1 cache. 

## How to reproduce the attacks

### Images 

To remake the images showing the patterns of each attack you can use the `heatmap.py` using the `.out` file in each data folder.

### Raw Data 

To obtain the raw data again you can use the already synthesized components. Using Vitis IDE you have to build an application using as the platform the `.xsa` file present in each attack folder. Then you need to import the controller code and the `aes.h` header. You also need to change the heap size inside the linker script (`lscript.ld`) to `0x80000`.

### Re-Build everything

- Create a new project in Vitis HLS (select the Zynq-7 ZC702 Evaluation Board), and load the core.cpp file and synthesize the component
- Export the component (Export RTL)
- Create a new project in Vivado (select the Zedboard board), add the Custom IP inside the IP catalog (Add Repository...)
- Add the VHDL code for the timer and the evicter
- Build the block design with the script in each attack folder
- Generate the Bitstream and export it
- After this, follow the [Raw Data] steps

## Software Versions

The software used to develop the attacks is listed below:

- Vivado v2022.1
- Xilinx Vitis IDE v2022.1
- Xilinx Vitis HLS v2022.1