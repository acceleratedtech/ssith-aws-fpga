# SSITH AWS FPGA

This repository contains the host software for controlling a RISC-V processor running on Amazon AWS F1 FPGA.

## How to run it:

Build FPGA utilities and set them up to run as root:
```bash
   cd aws-fpga
   . hdk_setup
   sudo chmod u+s /usr/local/bin/fpga-local-cmd
```

Install connectal and load drivers
```bash
  sudo add-apt-repository -y ppa:jamey-hicks/connectal
  sudo apt-get install connectal
  fpga-clear-local-fpga -S 0
  modprobe portalmem
  modprobe pcieportal
```

Build `ssith_aws_fpga`

```bash
   mkdir build
   cd build
   cmake ..
   make
   cd ..
   dtc -I dts -O dtb -o build/devicetree.dtb src/dts/devicetree.dts
   fpga-load-local-image -S 0 -I agfi-01019fdac375031e7
   ./build/ssith_aws_fpga --dtb build/devicetree.dtb  --elf foo.elf --entry=0x80003000
```
