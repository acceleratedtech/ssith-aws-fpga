cmake_minimum_required(VERSION 3.10)
project(ssith_aws_fpga)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(src/connectal)
include_directories(src/generated)

add_executable(ssith_aws_fpga
        src/connectal/DmaBuffer.cpp
        src/connectal/DmaBuffer.h
        src/connectal/dmaManager.c
        src/connectal/dmaManager.h
        src/connectal/dmaSendFd.h
        # src/connectal/kernel_module.c
        # src/connectal/manualMMUIndication.h
        # src/connectal/MMUServer.h
        # src/connectal/monkit.h
        src/connectal/platformMemory.cpp
        src/connectal/poller.cpp
        src/connectal/portal.c
        src/connectal/portal.h
        # src/connectal/portalJson.c
        # src/connectal/portalKernel.h
        src/connectal/portalPrintf.c
        # src/connectal/portalPython.cpp
        # src/connectal/runpython.cpp
        # src/connectal/sock_utils.c
        # src/connectal/sock_utils.h
        src/connectal/timer.c
        # src/connectal/TlpReplay.cpp
        src/connectal/transportHardware.c
        # src/connectal/transportPortal.c
        # src/connectal/transportSerial.c
        # src/connectal/transportShared.c
        # src/connectal/transportSocket.c
        # src/connectal/transportWebSocket.c
        # src/connectal/transportXsim.c

        src/generated/AWSP2_Request.c
        src/generated/AWSP2_Request.h
        src/generated/AWSP2_RequestJson.cpp
        src/generated/AWSP2_Response.c
        src/generated/AWSP2_Response.h
        src/generated/AWSP2_ResponseJson.cpp
        src/generated/ConnectalProjectConfig.h
        src/generated/driver_signature_file.h
        src/generated/GeneratedCppCallbacks.cpp
        src/generated/GeneratedTypes.h
        src/generated/MemServerIndication.c
        src/generated/MemServerIndication.h
        src/generated/MemServerIndicationJson.cpp
        src/generated/MemServerRequest.c
        src/generated/MemServerRequest.h
        src/generated/MemServerRequestJson.cpp
        src/generated/MMUIndication.c
        src/generated/MMUIndication.h
        src/generated/MMUIndicationJson.cpp
        src/generated/MMURequest.c
        src/generated/MMURequest.h
        src/generated/MMURequestJson.cpp
        src/generated/topEnum.h
        src/fpga.h
        src/loadelf.cpp
        src/loadelf.h
        src/testawsp2.cpp)
target_link_libraries(ssith_aws_fpga pthread elf)
