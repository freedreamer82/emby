set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

message("${CMAKE_BUILD_TYPE}")
# project settings

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(EMBY_FOLDER ../../../)
set(EMBY_PLATFORM X86_Unix)

if (EMBY_FOLDER)
    message(STATUS "including EMBY cmake:" ${EMBY_FOLDER}/emby.cmake)
    set(ENV{EMBY_FOLDER} ["${EMBY_FOLDER}"])
    include("${EMBY_FOLDER}/emby.cmake")
endif()

## specify cross compilers and tools
#set(CMAKE_C_COMPILER arm-none-eabi-gcc)
#set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
#set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
#set(CMAKE_AR arm-none-eabi-ar)
#set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
#set(CMAKE_OBJDUMP arm-none-eabi-objdump)
#set(SIZE arm-none-eabi-size)
#set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
#

#Uncomment for hardware floating point
#add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)

#set(COMPILE_OPTS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mfloat-abi=hard )

add_compile_options("${COMPILE_OPTS}")
add_link_options("${COMPILE_OPTS}")
set(CMAKE_EXE_LINKER_FLAGS  "${COMPILE_OPTS}")

if(!APPLE)
   add_compile_options(-fsingle-precision-constant)
endif()
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0  -Werror=return-type)

# uncomment to mitigate c++17 absolute addresses warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics ")

# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)


#definition for project
add_compile_definitions(EMBY_BUILD_x86)
add_compile_options(-Os -g)

#get absolute path from Emby rotto path from here
get_filename_component(ROOT_DIR "../../../" ABSOLUTE)
message(STATUS ${ROOT_DIR})

message(STATUS ${PROJECT_SOURCE_DIR})

include_directories( ${ROOT_DIR}/PlatformSpecific/X86_Unix/EmbySystem/
                     ${ROOT_DIR}/PlatformAgnostic/)

#project def goes AFTER TOOLCHAIN SETUP!!!! otherwise doesn't recognize correctly gcc
project(test-buffer CXX ASM)

file(GLOB_RECURSE SOURCES
        "Test_Buffer.cc"
        "${ROOT_DIR}/PlatformSpecific/X86_Unix/EmbySystem/System.cc")

#add_link_options(-T ${LINKER_SCRIPT})
#add_link_options(  -lc -lm -lnosys  )

add_executable(${PROJECT_NAME} ${SOURCES})

## report size
function(show_object_size target)
    string(REPLACE "objcopy" "size" CMAKE_OBJSIZE "${CMAKE_OBJCOPY}")
    message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf)
    message(STATUS ${CMAKE_OBJSIZE})

    add_custom_command(
            TARGET ${target} POST_BUILD
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_OBJSIZE} --format=berkeley --totals ${CMAKE_CURRENT_BINARY_DIR}/${target}
    )
endfunction()

#show_object_size(${PROJECT_NAME})
