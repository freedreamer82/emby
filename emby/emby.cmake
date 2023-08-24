set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if (EMBY_CONFIG)
    if(MSVC)
        add_definitions(/FI"${EMBY_CONFIG}")
    else()
        # GCC or Clang
        message(STATUS "EMBY Config:" "${EMBY_CONFIG}")
        add_definitions(-include "${EMBY_CONFIG}")
    endif()
endif()


if (EMBY_FOLDER)
    message(STATUS "EMBY folder:" "${EMBY_FOLDER}")
    include_directories(${EMBY_FOLDER}/Include/)
endif ()

if (EMBY_PLATFORM)
    include("${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/emby-platform.cmake")
    message(STATUS "PLATFORM: " ${EMBY_PLATFORM})
    include_directories(${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM})
    add_compile_definitions(EMBY_BUILD_${EMBY_PLATFORM})
endif()

include("${EMBY_FOLDER}/toolchain.cmake")

add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if(LINUX)
  add_compile_options(-fsingle-precision-constant)
endif()

add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Werror=return-type)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for size , no DEBUG")
    add_compile_options(-O0)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Os -g)
    add_compile_definitions(DEBUG BUILD_DEBUG)
endif ()

message(STATUS "${EMBY_FOLDER}" /PlatformAgnostic/EmbyConfig)
include_directories(${EMBY_FOLDER}/PlatformAgnostic)

file(GLOB_RECURSE EMBY_SOURCES
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyConfig/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyConsole/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyDebug/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyLibs/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyLog/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyMachine/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbySystem/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyThreading/*.cc"
        "${EMBY_FOLDER}/PlatformAgnostic/EmbyTime/*.cc"
        )


list (APPEND EMBY_SOURCES ${EMBY_PLAT_SOURCES})


if (EMBY_CMAKE_APPEND)
         # GCC or Clang
         include("${EMBY_CMAKE_APPEND}")
         message(STATUS "Including custom cmake: " ${EMBY_CMAKE_APPEND})
endif()

