#cmake . -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NDK=$NDK -DANDROID_PLATFORM=android-21 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_NDK=$NDK -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_BUILD_TYPE=Debug -GNinja

cmake_minimum_required(VERSION 3.22)

# set(VCPKG_ROOT_DIR $ENV{VCPKG_ROOT})
# set(VCPKG ${VCPKG_ROOT_DIR} CACHE STRING "path to the root folder of vcpkg")
# set(CMAKE_TOOLCHAIN_FILE "${VCPKG}/scripts/buildsystems/vcpkg.cmake")

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
option(MODULES "Enable Cpp20 Modules" ON)

# physics module
option(WOLF_PHYSX "Enable NVIDIA PhysX" OFF)

# render module
option(WOLF_RENDER "Enable real-time rendering based on bgfx" OFF)
option(WOLF_RENDER_RAYTRACING "Enable real-time ray tracing" OFF)

# media module
option(WOLF_MEDIA_AV1 "Enable AV1 video coding format" OFF)
option(WOLF_MEDIA_VP9 "Enable VP9 video coding format" OFF)
option(WOLF_MEDIA_WEBP "Enable WebP image format" OFF)

# stream module
option(WOLF_STREAM_GRPC "Enable gRPC connection" OFF)
option(WOLF_STREAM_KAFKA_CLIENT "Enable Kafka API connector" OFF)
option(WOLF_STREAM_NNG "Enable nng socket for tcp/udp/ipc client/server" OFF)
option(WOLF_STREAM_QUIC "Enable QUIC" OFF)
option(WOLF_STREAM_RIST "Enable RIST streaming protocol" OFF)
option(WOLF_STREAM_RTSP_CLIENT "Enable real time streaming protocol for client" OFF)
option(WOLF_STREAM_RTSP_SERVER "Enable real time streaming protocol for server" OFF)
option(WOLF_STREAM_SRT "Enable SRT streaming protocol" OFF)
option(WOLF_STREAM_WEBRTC_CLIENT "Enable webRTC client" OFF)
option(WOLF_STREAM_WEBRTC_SERVER "Enable webRTC server" OFF)
option(WOLF_STREAM_WEBSOCKET_CLIENT "Enable websocket for client" OFF)
option(WOLF_STREAM_WEBSOCKET_SERVER "Enable websocket for server" OFF)

# system module
option(WOLF_SYSTEM_AVX2 "Enable avx2" OFF)
option(WOLF_SYSTEM_AVX512 "Enable avx512" OFF)
option(WOLF_SYSTEM_BROTLI "Enable brotli compression" OFF)
option(WOLF_SYSTEM_EXECUTION "Enable Unified Executors based on libunifex" OFF)
option(WOLF_SYSTEM_FIBER "Enable fiber" OFF)
option(WOLF_SYSTEM_HTTP1_1_CLIENT "Enable http 1.1 client" OFF)
option(WOLF_SYSTEM_HTTP1_1_SERVER "Enable http 1.1 server" OFF)
option(WOLF_SYSTEM_HTTP2_CLIENT "Enable http 2 client" OFF)
option(WOLF_SYSTEM_HTTP2_SERVER "Enable http 2 server" OFF)
option(WOLF_SYSTEM_LUA "Enable lua scripting" OFF)
option(WOLF_SYSTEM_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_SYSTEM_LZMA "Enable LZMA Compression" OFF)
option(WOLF_SYSTEM_MIMALLOC "Enable Microsoft Mimalloc" OFF)
option(WOLF_SYSTEM_POSTGRESQL_CLIENT "Enable PostgreSQL client connector" OFF)
option(WOLF_SYSTEM_RAFT "Enable raft algorithm" OFF)
option(WOLF_SYSTEM_REDIS_CLIENT "Enable ScyllaDB client connector" OFF)
option(WOLF_SYSTEM_ROCKSDB_CLIENT "Enable ScyllaDB client connector" OFF)
option(WOLF_SYSTEM_SOCKET "Enable native socket for tcp/udp client/server" OFF)
option(WOLF_SYSTEM_SSL "Enable WOLF SSL" OFF)
option(WOLF_SYSTEM_WEBKIT "Enable WebKit" OFF)

# set type of library
set(LIBRARY_TYPE "STATIC" CACHE STRING "Library type")

# set C/CXX ios standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (MSVC)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "/EHsc")
else()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fcoroutines-ts -fexceptions")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

# fetch catch2 for tests
message("fetching https://github.com/catchorg/Catch2.git")
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.x
)

# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
  SOURCE_SUBDIR  include
)

set(LIBS )

if (WOLF_SYSTEM_MIMALLOC)
    message("fetching https://github.com/microsoft/mimalloc.git")
    FetchContent_Declare(
        mimalloc-static
        GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
        GIT_TAG        master
    )
   list(APPEND LIBS mimalloc-static)
endif()

# fetch libunifex
if (WOLF_SYSTEM_EXECUTION)
    message("fetching https://github.com/facebookexperimental/libunifex.git")
    FetchContent_Declare(
        unifex
        GIT_REPOSITORY https://github.com/facebookexperimental/libunifex.git
        GIT_TAG        main
    )
    list(APPEND LIBS unifex)
endif()

# setup basic includes
set(INCLUDES     
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# fetch boost pool
#include("cmake/fetch_boost_pool.cmake")

# fetch boost fiber or asio dependencies
if (WOLF_ENABLE_FIBER OR WOLF_ENABLE_SOCKET)
    include("cmake/fetch_boost_basics.cmake")
    
    if (WOLF_ENABLE_FIBER)
        include("cmake/fetch_boost_fiber.cmake")
    else()
        include("cmake/fetch_boost_asio.cmake")
    endif()

endif()

# # fetch libnng
# message("fetching https://github.com/nanomsg/nng.git")
# FetchContent_Declare(
#   libnng
#   GIT_REPOSITORY https://github.com/nanomsg/nng.git
#   GIT_TAG        master
# )

# change some options of CMAKE dependencies
# set(EXPAT_SHARED_LIBS OFF CACHE BOOL "EXPAT_SHARED_LIBS")
FetchContent_Populate(catch2)
FetchContent_Populate(gsl)

if (WOLF_SYSTEM_MIMALLOC)
    FetchContent_MakeAvailable(
        mimalloc-static)
endif()
if (WOLF_SYSTEM_EXECUTION)
    FetchContent_MakeAvailable(unifex)
endif()

# enabling clang-tidy
# can be enabled with .CLANG-TIDY from Visual Studio Code
# https://devblogs.microsoft.com/cppblog/visual-studio-code-c-december-2021-update-clang-tidy/
# can be enabled with .CLANG-TIDY from Visual Studio
# https://devblogs.microsoft.com/cppblog/code-analysis-with-clang-tidy-in-visual-studio/
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -format-style=file;)  

# add definitions
if (MSVC)
    add_definitions(
        -D_WIN32
        -DWIN32
    )
    list(APPEND LIBS Onecore)
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_definitions(-DDEBUG)
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
file(GLOB_RECURSE WOLF_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/export.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.cppm"
)
file(GLOB_RECURSE W_SYSTEM_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/system/gametime.cppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/execution.cppm"
    "${CMAKE_CURRENT_SOURCE_DIR}/system/system.cppm"
)
if (WOLF_SYSTEM_SOCKET)
    file(GLOB_RECURSE W_NET_ASIO_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/system/asio/*"
    )
endif()

# add includes & libraries
list(APPEND INCLUDES
  #${boost_pool_SOURCE_DIR}/include
  ${boost_preprocessor_SOURCE_DIR}/include
  ${catch2_SOURCE_DIR}/single_include
  ${gsl_SOURCE_DIR}/include
)


if (WOLF_SYSTEM_MIMALLOC)
    list(APPEND INCLUDES
        ${mimalloc_static_SOURCE_DIR}/include
    )
endif()

if (WOLF_SYSTEM_EXECUTION)
    list(APPEND INCLUDES
      ${unifex_SOURCE_DIR}/include
    )
endif()

# includes
include_directories(${INCLUDES})

# add source codes
add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${WOLF_INCLUDE}
    ${WOLF_SRC}
    ${W_SYSTEM_SRC}
    ${W_SYSTEM_ASIO_SRC}
)

if (WOLF_SYSTEM_SOCKET)
    add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
        ${W_SYSTEM_ASIO_SRC}
    )
endif()

# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# create source group
source_group("wolf" FILES ${WOLF_SRC})
if (WOLF_SYSTEM_SOCKET)
    source_group("system/asio" FILES ${W_SYSTEM_ASIO_SRC})
endif()
source_group("system" FILES ${W_SYSTEM_SRC})

# add compile options
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -std=c++2b -fPIC -Wall)
endif()
target_compile_definitions(${PROJECT_NAME} PRIVATE 
  FOLLY_ASSUME_NO_JEMALLOC 
  FOLLY_ASSUME_NO_TCMALLOC)

# build tests
if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
  add_executable (${TEST_PROJECT_NAME} test.cpp)
  target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME} )
  target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wall)
  if (MSVC)
    target_compile_options(${TEST_PROJECT_NAME} PRIVATE -std=c++2b)
  endif()
  target_compile_definitions(${TEST_PROJECT_NAME} PRIVATE 
    FOLLY_ASSUME_NO_JEMALLOC 
    FOLLY_ASSUME_NO_TCMALLOC)
endif()
