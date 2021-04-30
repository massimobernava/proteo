INCLUDE( FindPackageHandleStandardArgs )
cmake_minimum_required(VERSION 3.14)

project (proteo)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)
#find_package(Lua51 REQUIRED)
find_package(LuaJIT REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(JSONC REQUIRED)
#find_package(ENet REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ZeroMQ REQUIRED)

find_path( AVCODEC_INCLUDE_DIR libavcodec/avcodec.h )
find_library( AVCODEC_LIBRARY avcodec )
find_path( SWSCALE_INCLUDE_DIR libswscale/swscale.h )
find_library( SWSCALE_LIBRARY swscale )
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

include_directories(${SWSCALE_INCLUDE_DIR} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_GFX_INCLUDE_DIRS} ${LUAJIT_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS} ${JSONC_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${ZMQ_INCLUDE_DIRS}) #${ENET_INCLUDE_DIRS}

if(ANDROID)
	add_executable(proteo SHARED proteo.c)
else()
	add_executable(proteo proteo.c proteo_opencv.cpp)
endif()

target_link_libraries(proteo ${SWSCALE_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_GFX_LIBRARIES} ${OPENSSL_LIBRARIES} ${LUAJIT_LIBRARIES} ${CURL_LIBRARIES} ${JSONC_LIBRARIES} ${OpenCV_LIBS} ${ZMQ_LIBRARIES}) #${ENET_LIBRARIES}

#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_GENERATOR "DragNDrop")
#set(CPACK_PACKAGE_NAME "Proteo")
#set(CPACK_PACKAGE_VERSION,1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -latomic")

set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
set_property (TARGET proteo PROPERTY XCODE_SCHEME_ARGUMENTS " -g -d")
set_property (TARGET proteo PROPERTY XCODE_SCHEME_ADDRESS_SANITIZER TRUE)

include(CPack)

