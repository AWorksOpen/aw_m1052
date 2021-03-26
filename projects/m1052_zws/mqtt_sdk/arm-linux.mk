MESSAGE(STATUS "BUILD FOR ARM LINUX")
#across compiler setting
#include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOL_CHAIN_DIR /opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi)
set(CMAKE_C_COMPILER ${TOOL_CHAIN_DIR}/bin/arm-fsl-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${TOOL_CHAIN_DIR}/bin/arm-fsl-linux-gnueabi-g++)
#CMAKE_FORCE_C_COMPILER(${TOOL_CHAIN_DIR}/bin/arm-fsl-linux-gnueabi-gcc GNU)
#CMAKE_FORCE_CXX_COMPILER(${TOOL_CHAIN_DIR}/bin/arm-fsl-linux-gnueabi-g++ GUN)

set(CMAKE_FIND_ROOT_PATH ${TOOL_CHAIN_DIR})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
