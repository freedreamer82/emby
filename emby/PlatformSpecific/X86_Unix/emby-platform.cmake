set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

message(STATUS "including cmake from: " ${EMBY_PLATFORM})


set(ENV{EMBY_CROSS_BUILD_ARCH} "X86")
add_compile_definitions(EMBY_BUILD_X86)

#include all sbdirectories
SUBDIRLIST(SUBDIRS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/")
foreach(subdir ${SUBDIRS})
    include_directories(${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/${subdir})
endforeach()

file(GLOB_RECURSE EMBY_PLAT_SOURCES
"${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*.c"   
"${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*.cc"
"${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*/*.c" 
"${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*/*.cc")
list(APPEND SOURCES ${EMBY_PLAT_SOURCES})

#message(STATUS  "${EMBY_PLAT_SOURCES}")