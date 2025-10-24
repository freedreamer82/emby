set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(EMBY_X86_PLATFORM ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "including cmake from: " ${EMBY_X86_PLATFORM})


set(ENV{EMBY_CROSS_BUILD_ARCH} "X86")
add_compile_definitions(EMBY_BUILD_X86)

# remove emby root from include dirs
EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(${EMBY_X86_PLATFORM})

#include all sbdirectories
include_directories(${EMBY_X86_PLATFORM})

EMBY_INCLUDE_ONLY_IMPL_DIRS(${EMBY_X86_PLATFORM} TRUE)

file(GLOB_RECURSE EMBY_PLAT_SOURCES
"${EMBY_X86_PLATFORM}/*/*.c"
"${EMBY_X86_PLATFORM}/*/*.cc")

list(APPEND SOURCES ${EMBY_PLAT_SOURCES})
