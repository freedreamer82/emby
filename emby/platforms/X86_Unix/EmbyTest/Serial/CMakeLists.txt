set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)


#project def goes AFTER TOOLCHAIN SETUP!!!! otherwise doesn't recognize correctly gcc
include("${CMAKE_CURRENT_SOURCE_DIR}/../emby-test.cmake")

set(PRJ_NAME emby-test-serial )

project(${PRJ_NAME} C CXX ASM)

file(GLOB_RECURSE TEST_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Test_*.cc"
)

# Ensure the compiler can find core headers (EmbyMachine/Serial.hh resides under emby/core)
# and general emby includes. These paths are relative to this CMakeLists.txt.
#include_directories(
#        "${CMAKE_CURRENT_SOURCE_DIR}/../../../../core"
#        "${CMAKE_CURRENT_SOURCE_DIR}/../../.."
#)

list (APPEND SOURCES ${EMBY_PLAT_SOURCES} ${TEST_SOURCES} ${EMBY_SOURCES})

add_executable(${PRJ_NAME} ${SOURCES} )