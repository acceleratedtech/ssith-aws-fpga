#!/bin/sh

function fatal {
    echo "FATAL: $1"
    exit 1
}

aws s3 ls s3://aws-fpga > /dev/null || fatal "No access to S3 bucket aws-fpga. Check aws credentials or create the bucket"

. aws-fpga/hdk_setup.sh
export BLUESPECDIR=/opt/bluespec/lib
(cd src_AWS_P2_MIT; make gen.awsf1)
export BLUESPECDIR=$PWD/bluespec/lib
export CONNECTALDIR=$PWD/connectal
sudo docker run --rm -it --env BLUESPECDIR=$BLUESPECDIR -v $PWD:$PWD jameyhicks/connectal:latest cp -r /opt/bluespec $PWD
sudo docker run --rm -it --env BLUESPECDIR=$BLUESPECDIR -v $PWD:$PWD jameyhicks/connectal:latest make -j8 -C $PWD/src_AWS_P2_MIT/awsf1 verilog
cd src_AWS_P2_MIT/awsf1
/home/centos/ssith-aws-fpga/hw/connectal/scripts/aws/build.sh -strategy TIMING -clock_recipe_a A1 -clock_recipe_b B5


