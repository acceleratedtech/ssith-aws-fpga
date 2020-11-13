#!/bin/sh

. aws-fpga/hdk_setup.sh
export BLUESPECDIR=/opt/bluespec/lib
(cd src_AWS_P2_MIT; make gen.awsf1)
docker run -it --env BLUESPECDIR=$BLUESPECDIR -v $PWD:$PWD jameyhicks/connectal:latest make -j8 -C $PWD/src_AWS_P2_MIT/awsf1 verilog
cd src_AWS_P2_MIT/awsf1
CONNECTALDIR=/home/centos/ssith-aws-fpga/hw/connectal /home/centos/ssith-aws-fpga/hw/connectal/scripts/aws/build.sh -strategy TIMING -clock_recipe_a A1 -clock_recipe_b B5


