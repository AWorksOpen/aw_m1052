cmake_minimum_required(VERSION 3.5)

project(mqttproto)

if(ARM_LINUX)
include(arm-linux.mk)
endif()

#MQTT若需要使用加密端口，需要开启宏 WITH_MQTT_SSL

ADD_DEFINITIONS(
    -DZLG_IOT_MQTT_TX_BUF_LEN=1024
    -DZLG_IOT_MQTT_RX_BUF_LEN=1024
    -DZLG_IOT_MQTT_NUM_SUBSCRIBE_HANDLERS=20
    -DENABLE_IOT_INFO=true
    -DENABLE_IOT_WARN=true
    -DENABLE_IOT_ERROR=true
)

set(SYS_LIBS mbedtls mbedx509 mbedcrypto jsmn)
if(APPLE)
	list(APPEND SYS_LIBS -lm -lpthread)
    ADD_DEFINITIONS(
        -DMACOS -g -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
        -Wno-deprecated -Wwrite-strings
    )
elseif(UNIX)
	list(APPEND SYS_LIBS -lm -lpthread)
    ADD_DEFINITIONS(
        -DUNIX -g -fPIC -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
        -Wno-deprecated -Wwrite-strings
    )
else()
	list(APPEND SYS_LIBS wsock32 ws2_32)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -DHAVE_STRUCT_TIMESPEC)
endif()

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(GTEST_DIR ${PROJECT_SOURCE_DIR}/3rd/googletest/googletest)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR} )

foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_DIR} )
    message(STATUS CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG})
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

link_directories(${BIN_DIR})
link_directories(${PROJECT_SOURCE_DIR}/3rd/jsmn)
link_directories(${PROJECT_SOURCE_DIR}/3rd/mbedtls/library)
link_directories(${PROJECT_SOURCE_DIR}/3rd/mbedtls/build/library)
include_directories(${PROJECT_SOURCE_DIR}/direct/protocol/common)

include_directories(${PROJECT_SOURCE_DIR}/direct/network/include)
include_directories(${PROJECT_SOURCE_DIR}/direct/network/platform/linux/common)
include_directories(${PROJECT_SOURCE_DIR}/direct/network/platform/linux/socket)
include_directories(${PROJECT_SOURCE_DIR}/direct/network/platform/linux/pthread)
include_directories(${PROJECT_SOURCE_DIR}/3rd/mbedtls/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd/jsmn)

add_subdirectory(3rd/jsmn)
add_subdirectory(direct)
#add_subdirectory(gateway)

