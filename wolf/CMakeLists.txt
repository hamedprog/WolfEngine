#cmake . -B build -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_NDK=$NDK -DANDROID_PLATFORM=android-21 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a -DCMAKE_ANDROID_NDK=$NDK -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=21 -DCMAKE_BUILD_TYPE=Debug -GNinja

cmake_minimum_required(VERSION 3.22)

# we will use fbstring from facebook folly

#Include vcpkg toolchain
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
option(WOLF_ENABLE_AVX2 "Enable avx2" OFF)
option(WOLF_ENABLE_AVX512 "Enable avx512" OFF)
option(WOLF_ENABLE_BROTLI "Enable brotli compression" OFF)
option(WOLF_ENABLE_FIBER "Enable fiber" ON)
option(WOLF_ENABLE_gRPC "Enable gRPC connection" OFF)
option(WOLF_ENABLE_HTTP_CLIENT "Enable http 1.1/2 request" OFF)
option(WOLF_ENABLE_HTTP_SERVER "Enable http 1.1/2 server" OFF)
option(WOLF_ENABLE_KAFKA_CLIENT "Enable Kafka API connector" OFF)
option(WOLF_ENABLE_LUA "Enable lua scripting" OFF)
option(WOLF_ENABLE_LZ4 "Enable LZ4 Compression" OFF)
option(WOLF_ENABLE_LZMA "Enable LZMA Compression" OFF)
option(WOLF_ENABLE_NNG "Enable nng socket for tcp/udp/ipc client/server" OFF)
option(WOLF_ENABLE_QUIC "Enable QUIC" OFF)
option(WOLF_ENABLE_REDIS_CLIENT "Enable ScyllaDB connector" OFF)
option(WOLF_ENABLE_RENDER "Enable real-time rendering based on bgfx" OFF)
option(WOLF_ENABLE_ROCKSDB_CLIENT "Enable ScyllaDB connector" OFF)
option(WOLF_ENABLE_RTSP_CLIENT "Enable real time streaming for client" OFF)
option(WOLF_ENABLE_RTSP_SERVER "Enable real time streaming for server" OFF)
option(WOLF_ENABLE_SKYLLADB_CLIENT "Enable ScyllaDB connector" OFF)
option(WOLF_ENABLE_SOCKET "Enable native socket for tcp/udp client/server" ON)
option(WOLF_ENABLE_SSL "Enable WOLF SSL" OFF)
option(WOLF_ENABLE_WEBRTC "Enable webRTC" OFF)
option(WOLF_ENABLE_WEBSOCKET_CLIENT "Enable websocket" OFF)
option(WOLF_ENABLE_WEBSOCKET_SERVER "Enable websocket" OFF)

# set type of library
set(LIBRARY_TYPE "SHARED" CACHE STRING "Library type")

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

# fetch gsl
message("fetching https://github.com/microsoft/GSL.git")
FetchContent_Declare(
  gsl
  GIT_REPOSITORY https://github.com/microsoft/GSL.git
  GIT_TAG        main
  SOURCE_SUBDIR  include
)
# fetch fmt
message("fetching https://github.com/fmtlib/fmt.git")
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        master
)
# fetch libunifex
message("fetching https://github.com/facebookexperimental/libunifex.git")
FetchContent_Declare(
  unifex
  GIT_REPOSITORY https://github.com/facebookexperimental/libunifex.git
  GIT_TAG        main
)
# fetch preprocessor for folly
message("fetching https://github.com/boostorg/preprocessor.git")
FetchContent_Declare(
  boost_preprocessor
  GIT_REPOSITORY https://github.com/boostorg/preprocessor.git
  GIT_TAG        master
)
# fetch catch2 for tests
message("fetching https://github.com/catchorg/Catch2.git")
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.x
)
# setup basic includes
set(INCLUDES     
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# setup basic libraries
set(LIBS 
  fmt 
  unifex
  boost_preprocessor)

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

FetchContent_MakeAvailable(boost_preprocessor)
FetchContent_Populate(gsl)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(unifex)
FetchContent_Populate(catch2)

# enabling clang-tidy
# can be enabled with .CLANG-TIDY from Visual Studio Code
# https://devblogs.microsoft.com/cppblog/visual-studio-code-c-december-2021-update-clang-tidy/
# can be enabled with .CLANG-TIDY from Visual Studio
# https://devblogs.microsoft.com/cppblog/code-analysis-with-clang-tidy-in-visual-studio/
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -format-style=file;)  

# set defenitions

if (MSVC)
add_definitions(-D_WIN32)
elseif (APPLE)
add_definitions(
  -DTARGET_IPHONE_SIMULATOR=0
  -DTARGET_OS_SIMULATOR=0
  -DTARGET_OS_IPHONE=0)
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
add_definitions(-DDEBUG)
else()
add_definitions(-DNDEBUG)
endif()

if (WOLF_ENABLE_FIBER)
add_definitions(-DWOLF_ENABLE_FIBER)
endif()

if (WOLF_ENABLE_SOCKET)
add_definitions(-DWOLF_ENABLE_SOCKET)
list(APPEND INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio
)
endif()

# include sources
file(GLOB_RECURSE WOLF_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/wolf.hpp"
)
file(GLOB_RECURSE W_MEMORY_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_memory/*"
)
file(GLOB_RECURSE W_NETWORK_ASIO_SRC_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/w_network/asio/*"
)

# add includes & libraries
list(APPEND INCLUDES
  ${boost_preprocessor_SOURCE_DIR}/include
  ${catch2_SOURCE_DIR}/single_include
  ${fmt_SOURCE_DIR}/include
  ${gsl_SOURCE_DIR}/include
  ${unifex_SOURCE_DIR}/include
)
include_directories(${INCLUDES})

# add source codes
add_library(${PROJECT_NAME} ${LIBRARY_TYPE}
    ${WOLF_SRC_DIR}
    ${W_MEMORY_SRC_DIR}
    ${W_NETWORK_ASIO_SRC_DIR}
)
# link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

# create source group
source_group("wolf" FILES ${WOLF_SRC_DIR})
source_group("w_memory" FILES ${W_MEMORY_SRC_DIR})
source_group("w_network/asio" FILES ${W_NETWORK_ASIO_SRC_DIR})

# add compile options
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++2b -fPIC -Wall)
target_compile_definitions(${PROJECT_NAME} PRIVATE 
  FOLLY_ASSUME_NO_JEMALLOC 
  FOLLY_ASSUME_NO_TCMALLOC)

# build tests
if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Android")
  add_executable (${TEST_PROJECT_NAME} test.cpp)
  target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${PROJECT_NAME} )
  target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wall -std=c++2b)
  target_compile_definitions(${TEST_PROJECT_NAME} PRIVATE 
    FOLLY_ASSUME_NO_JEMALLOC 
    FOLLY_ASSUME_NO_TCMALLOC)
endif()
