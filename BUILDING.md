# MIT SSITH P2 processor on Amazon AWS F1 FPGAs

This directory contains a configuration of the SSITH P2 processor that
runs on Amazon AWS F1 FPGAs. It uses
[Connectal](https://connectal.org), which provides several
capabilities:

1. Host software / FPGA message passing, where the messages are declared as BSV interfaces
2. Sharing of host memory between host software and RISC-V processor in the FPGA
3. Verilator simulation of this configuration
4. Unified build scipts for both Verilator and Amazon F1 builds

## Setting up for AWS builds

1. You need an AWS account

2. The build depends on AWS credentials being available in `~/.aws` or environment variables

3. The build uploads files to S3 bucket `s3:///aws-fpga/` and directs AWS to write some logs to `s3://aws-logs`

4. You should be able to use your own machines for builds using Vivado
2019.2 or you can use Amazon servers. Amazon's FPGA developer AMI is
centos-based.

## Installing build dependences

1. Install connectal:

  Option 1) Install from source:
  
  - `$ git clone https://github.com:/cambridgehackers/connectal`
  - `$ git clone https://github.com:/cambridgehackers/buildcache`
  - `$ git clone https://github.com:/cambridgehackers/fpgamake`
  - `$ sudo apt-get install python-ply verilator

  Option 2) Install from ppa (on Ubuntu xenial or bionic or request your release to be added to the PPE)

  - `$ sudo add-apt-repository -y ppa:jamey-hicks/connectal`
  - `$ sudo apt-get update`
  - `$ sudo apt-get install connectal`

2. Install aws-fpga

  - `$ git clone git://github.com/aws/aws-fpga`
  - `(cd aws-fpga; . hdk_setup.sh)`

3. Install other dependences

  - `$ sudo apt-get install libelf-dev libssl-dev libcurl4-openssl-dev `

## Building the Verilator simulator

  - `$ cd hw/src_AWS_P2; make -j8 build.verilator

## Running the Verilator simulation

  - `$ cd hw/src_AWS_P2; ./verilator/bin/ubuntu.exe --elf /path/to/elf

## Building the FPGA image

  - `$ cd hw/src_AWS_P2; make -j8 build.awsf1
  - go do something else for a few hours
  - when the build is done, it calls `aws ec2 create-fpga-image ...`
  - The last step is creating a partial reconfiguration image, which AWS does in some hidden place
  - The build script will write the output from `create-fpga-image` into `latest-fpga-image.json`

```JSON
    {
    "FpgaImageId": "afi-07db2f5d27a09ee33",
    "FpgaImageGlobalId": "agfi-0215f9a0be924fb7c"
    }
```

  - To see when your FPGA image is available, you can run this script which runs `aws ec2 describe-fpga-images`:
  - `$ connectal/scripts/aws/describe-latest-fpga-image.sh `
```JSON
    {
        "FpgaImages": [
            {
                "FpgaImageId": "afi-0579dc72622e71803",
                "FpgaImageGlobalId": "agfi-097924a39b3e2bb09",
                "Name": "awsp2",
                "Description": "20_03_27-010910",
                "ShellVersion": "0x04261818",
                "PciId": {
                    "DeviceId": "0xf000",
                    "VendorId": "0x1d0f",
                    "SubsystemId": "0x1d51",
                    "SubsystemVendorId": "0xfedd"
                },
                "State": {
                    "Code": "available"
                },
                "CreateTime": "2020-03-27T11:28:02.000Z",
                "UpdateTime": "2020-03-27T12:04:03.000Z",
                "OwnerId": "128857099957",
                "Tags": [],
                "Public": false
            }
        ]
    }

## Setting up the Amazon F1 server

For this step, you need an AWS F1 instance and you'll need the
connectal device drivers built and installed. It's much easier to do
this from pre-built packages, so I recommend running Ubuntu 18.04 on
the F1 instance.

1. Install connectal on the server

  - `$ sudo add-apt-repository -y ppa:jamey-hicks/connectal`
  - `$ sudo apt-get update`
  - `$ sudo apt-get install connectal`

2. Check out the repo

3. Build the host software

  - `$ cd ssith-aws-fpga; mkdir build; cd build; cmake ..; make -j8`

## Running the FPGA

1. Load the drivers

  - `$ modprobe pcieportal`
  - `$ modprobe portalmem`

2. Load the FPGA image:

  - `$ sudo fpga-load-local-image -S 0 -I agfi-097924a39b3e2bb09`

3. Run the host software, which loads code into the FPGA and handles I/O requests from the RISC-V CPU:

  - `$ cd hw/src_AWS_P2; ./awsfs1/bin/ubuntu.exe --elf /path/to/elf`

