set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)


#project def goes AFTER TOOLCHAIN SETUP!!!! otherwise doesn't recognize correctly gcc
include("../emby-test.cmake")

set(PRJ_NAME emby-test-buffer )

project(${PRJ_NAME} C CXX ASM)

file(GLOB_RECURSE TEST_SOURCES
        "Test_Buffer.cc")

list (APPEND SOURCES ${EMBY_PLAT_SOURCES} ${TEST_SOURCES} ${EMBY_SOURCES})

add_executable(${PRJ_NAME} ${SOURCES} )