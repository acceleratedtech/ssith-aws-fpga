cmake_minimum_required(VERSION 3.10)
project(ssith_aws_fpga)

if(FPGA)
message(NOTICE " Building for FPGA")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDEFAULT_NOPROGRAM=1 -DBOARD_awsf1 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -DDEFAULT_NOPROGRAM=1 -DBOARD_awsf1 -g")
else()
message(NOTICE " Building for simulation")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSIMULATION -DBOARD_verilator -g")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -DSIMULATION -DBOARD_verilator -g")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DGDBSTUB_NO_ELF_LOAD")

add_subdirectory(src/tinyemu)
add_subdirectory(src/connectal)
add_subdirectory(src/generated)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(src/tinyemu)
include_directories(src/connectal)
include_directories(src/generated)
include_directories(src/RISCV_gdbstub/src)

add_executable(ssith_aws_fpga
  src/fpga.h
  src/fpga.cpp
  src/loadelf.cpp
  src/loadelf.h
  src/util.cpp
  src/util.h
  src/virtiodevices.cpp
  src/virtiodevices.h
  src/RISCV_gdbstub/src/RVDM.c
  src/RISCV_gdbstub/src/gdbstub.c
  src/RISCV_gdbstub/src/gdbstub_be.c
  src/RISCV_gdbstub/src/gdbstub_fe.c
  src/main.cpp)
target_link_libraries(ssith_aws_fpga tinyemu connectal generated pthread elf)

add_executable(ssith_virtio_test
  src/fpga.h
  src/fpga.cpp
  src/loadelf.cpp
  src/loadelf.h
  src/util.cpp
  src/util.h
  src/virtiodevices.cpp
  src/virtiodevices.h
  src/RISCV_gdbstub/src/RVDM.c
  src/RISCV_gdbstub/src/gdbstub.c
  src/RISCV_gdbstub/src/gdbstub_be.c
  src/RISCV_gdbstub/src/gdbstub_fe.c
  src/test/virtiotest.cpp
  )
target_link_libraries(ssith_virtio_test tinyemu connectal generated pthread elf)

