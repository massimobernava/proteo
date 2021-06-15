INCLUDE( FindPackageHandleStandardArgs )
cmake_minimum_required(VERSION 3.14)

OPTION (LUA_USE_LUAJIT "Use LuaJIT instead of default Lua" ON)

project (proteo_server)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

find_package(LuaJIT REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(JSONC REQUIRED)
find_package(libmicrohttpd REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(EJDB)
find_package (Threads)
find_package(OpenCV REQUIRED)
find_package(ZeroMQ REQUIRED)
#find_package(TensorFlow REQUIRED)

find_path( AVCODEC_INCLUDE_DIR libavcodec/avcodec.h )
find_library( AVCODEC_LIBRARY avcodec )
find_path( SWSCALE_INCLUDE_DIR libswscale/swscale.h )
find_library( SWSCALE_LIBRARY swscale )
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(TensorFlowLite_INCLUDE_DIR
NAMES
tensorflow/lite
tensorflow
HINTS
${CMAKE_SOURCE_DIR}/tflite/include
/usr/local/include)

find_library(TensorFlowLite_LIBRARY
NAMES
tensorflowlite_c
tensorflowlite
HINTS
${CMAKE_SOURCE_DIR}/tflite/lib
/usr/lib
/usr/local/lib)

find_path(DeepSpeech_INCLUDE_DIR
NAMES
deepspeech
HINTS
${CMAKE_SOURCE_DIR}/deepspeech/include
/usr/local/include)

find_library(DeepSpeech_LIBRARY
NAMES
deepspeech
HINTS
${CMAKE_SOURCE_DIR}/deepspeech/lib
/usr/lib
/usr/local/lib)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(DEEPSPEECH DEFAULT_MSG DeepSpeech_INCLUDE_DIR)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TFLITE DEFAULT_MSG TensorFlowLite_INCLUDE_DIR)

if (OpenCV_VERSION_MAJOR GREATER 4)
	add_definitions(-DOPENCV_DNN)
endif()

if(ejdb2_FOUND)
	include_directories(${EJDB_INCLUDE_DIRS})
	add_definitions(-DEJDB2)
endif()

if(DEEPSPEECH_FOUND)
	include_directories(${DeepSpeech_INCLUDE_DIR})
	add_definitions(-DDEEPSPEECH)
endif()

if(TFLITE_FOUND)
    include_directories(${TensorFlowLite_INCLUDE_DIR})
    add_definitions(-DTFLITE)
endif()

include_directories(${AVUTIL_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR} ${AVCODEC_INCLUDE_DIR} ${LUAJIT_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS} ${JSONC_INCLUDE_DIRS} ${LIBMICROHTTPD_INCLUDE_DIRS} ${SQLite3_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${ZMQ_INCLUDE_DIRS})

#include_directories(${CMAKE_SOURCE_DIR}/darknet)
#link_directories(${CMAKE_SOURCE_DIR}/darknet)

#link_directories(${CMAKE_SOURCE_DIR}/usr/local/opt/curl/lib)

add_executable(proteo_server proteo_server.c proteo_opencv.cpp)

if(ejdb2_FOUND)
	target_link_libraries(proteo_server ${EJDB_LIBRARIES})
endif()

if(DEEPSPEECH_FOUND)
	target_link_libraries(proteo_server ${DeepSpeech_LIBRARY})
endif()

if(TFLITE_FOUND)
    target_link_libraries(proteo_server ${TensorFlowLite_LIBRARY})
endif()

target_link_libraries(proteo_server ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY} ${AVCODEC_LIBRARY} ${OPENSSL_LIBRARIES} ${LUAJIT_LIBRARIES} ${CURL_LIBRARIES} ${JSONC_LIBRARIES} ${LIBMICROHTTPD_LIBRARIES} ${SQLite3_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} ${ZMQ_LIBRARIES})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -latomic")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS libluajit-5.1-dev ,libssl-dev,libcurl4-openssl-dev,libjson-c-dev,libmicrohttpd-dev,libsqlite3-dev,ejdb2,libopencv-dev,libzmq3-dev,libavcodec-dev,libavformat-dev,libavutil-dev,libswresample-dev,libswscale-dev)
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "M")
INCLUDE(CPack)
