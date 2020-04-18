cmake_minimum_required(VERSION 3.10)
project(ssith_aws_fpga)

set(SIMULATION 1)

if(SIMULATION)
message(NOTICE " Building for simulation")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSIMULATION -DBOARD_verilator")
else()
message(NOTICE " Building for FPGA")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEFAULT_NOPROGRAM=1 -DBOARD_awsf1")
endif()

add_subdirectory(src/tinyemu)
add_subdirectory(src/connectal)
add_subdirectory(src/generated)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(src/tinyemu)
include_directories(src/connectal)
include_directories(src/generated)

add_executable(ssith_aws_fpga
  src/fpga.h
  src/fpga.cpp
  src/loadelf.cpp
  src/loadelf.h
  src/virtiodevices.cpp
  src/virtiodevices.h
  src/testawsp2.cpp)
target_link_libraries(ssith_aws_fpga tinyemu connectal generated pthread elf)

add_executable(ssith_virtio_test
  src/fpga.h
  src/fpga.cpp
  src/loadelf.cpp
  src/loadelf.h
  src/virtiodevices.cpp
  src/virtiodevices.h
  src/test/virtiotest.cpp
  )
target_link_libraries(ssith_virtio_test tinyemu connectal generated pthread elf)

