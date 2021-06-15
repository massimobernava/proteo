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
find_package(OpenGL)
find_package(GLEW 2.0)

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

FIND_PACKAGE_HANDLE_STANDARD_ARGS(TFLITE DEFAULT_MSG TensorFlowLite_INCLUDE_DIR)

if(TFLITE_FOUND)
    include_directories(${TensorFlowLite_INCLUDE_DIR})
    add_definitions(-DTFLITE)
endif()

if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
    add_definitions(-DGLEW)
endif()

include_directories( ${SWSCALE_INCLUDE_DIR} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_GFX_INCLUDE_DIRS} ${LUAJIT_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR} ${CURL_INCLUDE_DIRS} ${JSONC_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${ZMQ_INCLUDE_DIRS}) #${ENET_INCLUDE_DIRS}

if(ANDROID)
	add_executable(proteo SHARED proteo.c)
elseif(APPLE)
    set(ICON_NAME "Proteo.icns")
    set(ICON_PATH ${CMAKE_SOURCE_DIR}/${ICON_NAME})
    set(proteo_ICON ${ICON_PATH})
    set_source_files_properties(${proteo_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(FONT1_NAME "ColabReg.otf")
    set(FONT1_PATH ${CMAKE_SOURCE_DIR}/${FONT1_NAME})
    set(proteo_FONT1 ${FONT1_PATH})
    set_source_files_properties(${proteo_FONT1} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(FONT2_NAME "OpenSans-Regular.ttf")
    set(FONT2_PATH ${CMAKE_SOURCE_DIR}/${FONT2_NAME})
    set(proteo_FONT2 ${FONT2_PATH})
    set_source_files_properties(${proteo_FONT2} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(CONFIG_NAME "config.json")
    set(CONFIG_PATH ${CMAKE_SOURCE_DIR}/${CONFIG_NAME})
    set(proteo_CONFIG ${CONFIG_PATH})
    set_source_files_properties(${proteo_CONFIG} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    file(COPY ${ICON_PATH} DESTINATION "proteo.app/Contents/Resources")
    add_executable(proteo MACOSX_BUNDLE ${ICON_PATH} ${FONT1_PATH} ${FONT2_PATH} ${CONFIG_PATH} proteo.c proteo_opencv.cpp)
    set_target_properties(proteo PROPERTIES MACOSX_BUNDLE_ICONFILE ${ICON_NAME})
    set_target_properties(proteo PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/MacOSXBundleInfo.plist.in)

    SET(APPS "\${CMAKE_INSTALL_PREFIX}/proteo.app")
    
    #Copia il file nella directory principale
    #INSTALL(TARGETS proteo BUNDLE DESTINATION ${CMAKE_SOURCE_DIR} COMPONENT Runtime)

    #set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})
    install(CODE "
    
    MESSAGE(\"Fixup\")
    include(BundleUtilities)
    fixup_bundle(${CMAKE_SOURCE_DIR}/install/proteo.app \"\" \"\")
    
    " COMPONENT Runtime)
else()
	add_executable(proteo proteo.c proteo_opencv.cpp)
endif()

if(TFLITE_FOUND)
    target_link_libraries(proteo ${TensorFlowLite_LIBRARY})
endif()

if (GLEW_FOUND)
    target_link_libraries(proteo ${GLEW_LIBRARIES} ${OPENGL_gl_LIBRARY})
endif()

target_link_libraries(proteo  ${SWSCALE_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_GFX_LIBRARIES} ${OPENSSL_LIBRARIES} ${LUAJIT_LIBRARIES} ${CURL_LIBRARIES} ${JSONC_LIBRARIES} ${OpenCV_LIBS} ${ZMQ_LIBRARIES}) #${ENET_LIBRARIES}

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -latomic")

#set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
#set_property (TARGET proteo PROPERTY XCODE_SCHEME_ARGUMENTS " -g -d")
#set_property (TARGET proteo PROPERTY XCODE_SCHEME_ADDRESS_SANITIZER TRUE)

#set(CPACK_BUNDLE_NAME "Proteo")
#set(CPACK_BUNDLE_ICON "Proteo.icns")
#set(CPACK_BUNDLE_PLIST "Info.plist")

#include(CPack)



