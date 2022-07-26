#cmake . -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NDK=$NDK -DANDROID_PLATFORM=android-21 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_NDK=$NDK -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_BUILD_TYPE=Debug -GNinja

cmake_minimum_required(VERSION 3.22)

# set(VCPKG_ROOT_DIR $ENV{VCPKG_ROOT})
# set(VCPKG ${VCPKG_ROOT_DIR} CACHE STRING "path to the root folder of vcpkg")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG}/scripts/buildsystems/vcpkg.cmake")

set(WEBRTC_ROOT_DIR $ENV{WEBRTC_ROOT})
set(WEBRTC_SRC ${WEBRTC_ROOT_DIR} CACHE STRING "path to the root folder of webrtc src folder")

# set it to enviroment variable if it is not set
if ("${WEBRTC_SRC}" STREQUAL "")
  set(WEBRTC_SRC $ENV{WEBRTC_ROOT})
endif()

project(wolf)
set(TEST_PROJECT_NAME "${PROJECT_NAME}_tests")
message("CXX Compiler ID is ${CMAKE_CXX_COMPILER_ID}")

# required packages
find_package(Git REQUIRED)
if (LINUX)
    find_package(Wget REQUIRED)
    message("make sure libtool was installed.(apt-get install libtool-bin)")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if (WIN32)
        set(WIN64 TRUE)
    endif()
endif()

# FetchContent for cloning repositories, avaiable since CMAKE 3.11
include(FetchContent)

# CMAKE GUI Options

option(WOLF_TESTS "Enable tests" ON)

# media modules
# option(WOLF_MEDIA_AV1 "Enable AV1 video coding format" OFF)
# option(WOLF_MEDIA_IMAGE "Enable WebP/BMP/JPG/PNG formats" OFF)
# option(WOLF_MEDIA_SCREEN_CAPTURE "Enable screen capture" OFF)
# option(WOLF_MEDIA_VPX "Enable VPX video coding format" OFF)

# option(WOLF_RENDER "Enable cross platform render based on BGFX" OFF)

# stream modules
# option(WOLF_STREAM_GRPC "Enable gRPC connection" OFF)
# option(WOLF_STREAM_KAFKA_CLIENT "Enable Kafka API connector" OFF)
# option(WOLF_STREAM_NNG "Enable nng socket for tcp/udp/ipc client/server" OFF)
# option(WOLF_STREAM_QUIC "Enable QUIC protocol" OFF)
# option(WOLF_STREAM_RIST "Enable RIST streaming protocol" OFF)
# option(WOLF_STREAM_RTSP_CLIENT "Enable RTSP for client" OFF)
# option(WOLF_STREAM_RTSP_SERVER "Enable RTSP for server" OFF)
option(WOLF_STREAM_HTTP "Enable http 1.1/2 and websocket protocols" ON)
option(WOLF_STREAM_WEBRTC "Enable native webRTC" ON)

# system modules
# option(WOLF_SYSTEM_AVX2 "Enable avx2" OFF)
# option(WOLF_SYSTEM_AVX512 "Enable avx512" OFF)
# option(WOLF_SYSTEM_BROTLI "Enable brotli compression" OFF)
# option(WOLF_SYSTEM_LUA "Enable lua scripting" OFF)
# option(WOLF_SYSTEM_LZ4 "Enable LZ4 Compression" OFF)
# option(WOLF_SYSTEM_LZMA "Enable LZMA Compression" OFF)
# option(WOLF_SYSTEM_MIMALLOC "Enable Microsoft mimalloc" OFF)
# option(WOLF_SYSTEM_RAFT "Enable raft algorithm" OFF)

option(WOLF_SYSTEM_EXECUTION "Enable the prototype implementation of the C++ sender/receiver async programming model based on libunifex" OFF)
option(WOLF_SYSTEM_JSON "Enable json" ON)


# set type of library
set(LIBRARY_TYPE "SHARED" CACHE STRING "Library type")

# set C/CXX ios standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
  set(CMAKE_BUILD_TYPE "Debug")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

find_package (Threads)

if (MSVC)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "/EHsc /W4")
else()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fexceptions")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
  SOURCE_SUBDIR  include
)
FetchContent_Populate(gsl)

# fetch expected
# message("fetching https://github.com/TartanLlama/expected.git")
# FetchContent_Declare(
#   expected
#   GIT_REPOSITORY https://github.com/TartanLlama/expected.git
#   GIT_TAG        master
#   SOURCE_SUBDIR  include
# )
# FetchContent_Populate(expected)

set(INCLUDES     
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${gsl_SOURCE_DIR}/include
  #${expected_SOURCE_DIR}/include
)
set(LIBS)

if (NOT EMSCRIPTEN)
    list(APPEND LIBS Threads::Threads)
endif()

if (WOLF_SYSTEM_EXECUTION)
    # fetch libunifex
    message("fetching https://github.com/facebookexperimental/libunifex.git")
    FetchContent_Declare(
        unifex
        GIT_REPOSITORY https://github.com/facebookexperimental/libunifex.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(unifex)
    list(APPEND INCLUDES 
        ${unifex_SOURCE_DIR}/include
        ${unifex_BINARY_DIR}/include
    )
    list(APPEND LIBS unifex)
endif()

# fetch catch2 for tests
if (WOLF_TESTS)
    message("fetching https://github.com/catchorg/Catch2.git")
    FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.x
    )
    FetchContent_Populate(catch2)
    list(APPEND INCLUDES
        ${catch2_SOURCE_DIR}/single_include
    )
endif()

if (WOLF_SYSTEM_MIMALLOC)
    message("fetching https://github.com/microsoft/mimalloc.git")
    FetchContent_Declare(
        mimalloc-static
        GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(mimalloc-static)
    list(APPEND INCLUDES
        ${mimalloc_static_SOURCE_DIR}/include
    )
    list(APPEND LIBS mimalloc-static)
endif()

if (WOLF_STREAM_WEBRTC)
    # we need http api for webrtc

    if (NOT WOLF_SYSTEM_JSON)
        message( FATAL_ERROR "'WOLF_STREAM_WEBRTC' needs 'WOLF_SYSTEM_JSON'= ON" )
    endif()

    if (NOT WOLF_STREAM_HTTP)
        message( FATAL_ERROR "'WOLF_STREAM_WEBRTC' needs 'WOLF_STREAM_HTTP' = ON" )
    endif()
   
    list(APPEND INCLUDES
        ${WEBRTC_SRC}
        ${WEBRTC_SRC}/third_party/abseil-cpp
        ${WEBRTC_SRC}/third_party/libyuv/include
    )
    if (WIN32)
        add_definitions(
            -DWEBRTC_WIN 
            -D__PRETTY_FUNCTION__=__FUNCTION__ 
            -D_HAS_ITERATOR_DEBUGGING=0
            #-DUSE_X11 
            -DHAVE_SOUND 
            -D_WINSOCKAPI_)

        list(APPEND LIBS
            secur32 
            dmoguids 
            wmcodecdspuuid 
            strmiids 
            msdmo 
            winmm 
            dxgi 
            d3d11 
            iphlpapi 
            dwmapi
        )
    elseif (APPLE)
        add_definitions(
            -DHAVE_SOUND 
            -DWEBRTC_MAC 
            -DWEBRTC_POSIX 
            -fno-rtti)

        find_library(FOUNDATION Foundation)
        find_library(CORE_FOUNDATION CoreFoundation)
        find_library(APPLICATION_SERVICES ApplicationServices)
        find_library(CORE_SERVICES CoreServices)
        find_library(CORE_AUDIO CoreAudio)
        find_library(AUDIO_TOOLBOX AudioToolBox)

        list(APPEND LIBS
            #/Users/pooyaeimandar/Desktop/jsoncpp-master/build/lib/libjsoncpp.a
            ${FOUNDATION} 
            ${CORE_FOUNDATION} 
            ${APPLICATION_SERVICES} 
            ${CORE_SERVICES}
            ${CORE_AUDIO} 
            ${AUDIO_TOOLBOX} 
        )
    endif()
    add_definitions(-DHAVE_JPEG)
    link_directories(${WEBRTC_SRC}/out/${CMAKE_BUILD_TYPE}/obj/)
    list(APPEND LIBS webrtc)
endif()

if (WOLF_STREAM_HTTP)
    message("fetching https://github.com/civetweb/civetweb.git")
    FetchContent_Declare(
        civetweb
        GIT_REPOSITORY https://github.com/civetweb/civetweb.git
        GIT_TAG        master
    )
    set(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING")
    set(CIVETWEB_BUILD_TESTING OFF CACHE BOOL "CIVETWEB_BUILD_TESTING")
    set(CIVETWEB_ENABLE_CXX ON CACHE BOOL "CIVETWEB_ENABLE_CXX")
    set(CIVETWEB_ENABLE_ASAN OFF CACHE BOOL "CIVETWEB_ENABLE_ASAN")

    FetchContent_MakeAvailable(civetweb)

    list(APPEND INCLUDES
        ${civetweb_SOURCE_DIR}/include
    )
    link_directories(${civetweb_BINARY_DIR}/src/)
    list(APPEND LIBS civetweb-cpp)
    #add_definitions(-DOPENSSL_API_1_1 -DUSE_WEBSOCKET)
endif()

if (WOLF_SYSTEM_JSON)
    #if (MSVC)
    message("fetching https://github.com/open-source-parsers/jsoncpp.git")
    FetchContent_Declare(
        jsoncpp_static
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG        master
    )

    set(BUILD_OBJECT_LIBS OFF CACHE BOOL "BUILD_OBJECT_LIBS")        
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "BUILD_SHARED_LIBS")
    set(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING")
    set(JSONCPP_USE_SECURE_MEMORY ON CACHE BOOL "JSONCPP_USE_SECURE_MEMORY")
    set(JSONCPP_WITH_EXAMPLE OFF CACHE BOOL "JSONCPP_WITH_EXAMPLE")
    set(JSONCPP_WITH_TESTS OFF CACHE BOOL "JSONCPP_WITH_TESTS")
    set(JSONCPP_WITH_POST_BUILD_UNITEST OFF CACHE BOOL "JSONCPP_WITH_POST_BUILD_UNITEST")

    FetchContent_MakeAvailable(jsoncpp_static)

    list(APPEND INCLUDES
        ${jsoncpp_static_SOURCE_DIR}/include
    )
    list(APPEND LIBS jsoncpp_static)
    #else()
    #    # link jsoncpp to webrtc
    #    list(APPEND INCLUDES
    #        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsoncpp/include
    #    )
    #    if (APPLE)
    #        list(APPEND LIBS 
    #            ${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsoncpp/lib/mac/${CMAKE_BUILD_TYPE}/libjsoncpp.a
    #        )
    #    endif()
    #endif()
endif()

# enabling clang-tidy
# can be enabled with .CLANG-TIDY from Visual Studio Code
# https://devblogs.microsoft.com/cppblog/visual-studio-code-c-december-2021-update-clang-tidy/
# can be enabled with .CLANG-TIDY from Visual Studio
# https://devblogs.microsoft.com/cppblog/code-analysis-with-clang-tidy-in-visual-studio/
# set(CMAKE_CXX_CLANG_TIDY
#     clang-tidy;
#     -format-style=file;)  

# add definitions
if (MSVC)
    add_definitions(
        -EHsc 
        -DNOMINMAX
    )
    if (WIN64)
        add_definitions(-DWIN64)
    else()
        add_definitions(-DWIN32)
    endif()
elseif(APPLE)
    add_definitions(-DNEED_XLOCALE_H=1)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG -D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

# setup Wolf definitions
get_cmake_property(_vars VARIABLES)
foreach (_var ${_vars})
    string(FIND ${_var} "WOLF_" out)
    if(("${out}" EQUAL 0) AND ("(${${_var}}" MATCHES ON))
        add_definitions("-D${_var}")
    endif()
endforeach()

# include sources
file(GLOB_RECURSE WOLF_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/dllmain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DISABLE_ANALYSIS_BEGIN"
    "${CMAKE_CURRENT_SOURCE_DIR}/DISABLE_ANALYSIS_END"
)
file(GLOB_RECURSE WOLF_STREAM_WEBRTC_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/http/*"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/webrtc/*"
)
file(GLOB_RECURSE WOLF_SYSTEM_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/*"
)

# includes
include_directories(${INCLUDES})

# add source codes
add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${WOLF_SRCS}
    ${WOLF_STREAM_WEBRTC_SRC}
    ${WOLF_SYSTEM_SRC}
)

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# create source group
source_group("wolf" FILES ${WOLF_SRCS})
source_group("stream/webrtc" FILES ${WOLF_STREAM_WEBRTC_SRC})
source_group("system" FILES ${WOLF_SYSTEM_SRC})

# add compile options
if (NOT WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
endif()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)

# build tests
if (WOLF_TESTS)
    add_executable (${TEST_PROJECT_NAME} tests.cpp)
    link_directories(${CMAKE_BINARY_DIR})
    target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME})
    if (WOLF_SYSTEM_EXECUTION)
        target_link_libraries(${TEST_PROJECT_NAME} PRIVATE unifex)
    endif()
    if (NOT EMSCRIPTEN)
        target_link_libraries(${TEST_PROJECT_NAME} PRIVATE Threads::Threads)
    endif()
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -std=c++2b -Wall)
endif()