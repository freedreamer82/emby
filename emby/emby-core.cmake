set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(EMBY_FOLDER "${CMAKE_CURRENT_LIST_DIR}")
set(ENV{EMBY_FOLDER} "${EMBY_FOLDER}")
set(EMBY_CORE_FOLDER "${EMBY_FOLDER}/core")
set(ENV{EMBY_CORE_FOLDER} "${EMBY_CORE_FOLDER}")


include_directories("${EMBY_FOLDER}/Include")
include("${EMBY_FOLDER}/emby-utils.cmake")

message(STATUS "Emby Core cmake included from: " ${EMBY_FOLDER})

if (EMBY_CONFIG)
    if(MSVC)
        add_definitions(/FI"${EMBY_CONFIG}")
    else()
        # GCC or Clang
        message(STATUS "EMBY Config:" "${EMBY_CONFIG}")
        add_definitions(-include "${EMBY_CONFIG}")
    endif()
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

message(STATUS "${EMBY_FOLDER}" /core/EmbyConfig)
include_directories(${EMBY_CORE_FOLDER})

EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS(${EMBY_CORE_FOLDER} TRUE)

EMBY_UTILS_ADD_SOURCES(${EMBY_CORE_FOLDER}  EMBY_SOURCES)

if (EMBY_CMAKE_APPEND)
         # GCC or Clang
         include("${EMBY_CMAKE_APPEND}")
         message(STATUS "Including custom cmake: " ${EMBY_CMAKE_APPEND})
endif()
