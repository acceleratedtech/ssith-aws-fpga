include_directories(../generated)
include_directories(../connectal)
include_directories(..)

add_library(connectal
  DmaBuffer.cpp
  DmaBuffer.h
  dmaManager.c
  dmaManager.h
  dmaSendFd.h
  # kernel_module.c
  # manualMMUIndication.h
  # MMUServer.h
  # monkit.h
  platformMemory.cpp
  poller.cpp
  portal.c
  portal.h
  # portalJson.c
  # portalKernel.h
  portalPrintf.c
  # portalPython.cpp
  # runpython.cpp
  sock_utils.c
  sock_utils.h
  timer.c
  # TlpReplay.cpp
  transportHardware.c
  # transportPortal.c
  # transportSerial.c
  # transportShared.c
  transportSocket.c
  # transportWebSocket.c
  transportXsim.c)
