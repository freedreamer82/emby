# this one is important
if(LINUX)
    SET(CMAKE_SYSTEM_NAME Linux)
elseif(APPLE)
    SET(CMAKE_SYSTEM_NAME Darwin)
endif()

    #this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

set(TOOLCHAIN_PATH $ENV{CUSTOM_GCC_TOOLCHAIN_PATH})

if( DEFINED ENV{CUSTOM_GCC_TOOLCHAIN_PATH})
    message(STATUS "gcc toolchain TOOLCHAIN_PATH is " ${TOOLCHAIN_PATH})
else()
    message(STATUS "gcc toolchain not set , get default")
    set(TOOLCHAIN_PATH "")
endif()


if( DEFINED ENV{EMBY_CROSS_BUILD_ARCH})
    message(STATUS "Building for Arch " $ENV{EMBY_CROSS_BUILD_ARCH})
    if($ENV{EMBY_CROSS_BUILD_ARCH} STREQUAL "X86")
        set(PREFIX "")
    elseif($ENV{EMBY_CROSS_BUILD_ARCH} STREQUAL "ARM")
        set(PREFIX "arm-none-eabi-")
    else()
        message(FATAL_ERROR " only ARM and X86 are valid!")
    endif()
else()
#    message(FATAL_ERROR "EMBY_CROSS_BUILD_ARCH is not set!!")
endif()


# specify the cross compiler
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}${PREFIX}g++)
SET(CMAKE_ASM_COMPILER ${TOOLCHAIN_PATH}${PREFIX}gcc)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}${PREFIX}gcc)
SET(CMAKE_LINKER ${TOOLCHAIN_PATH}${PREFIX}gcc)
SET(CMAKE_CPP_COMPILER ${TOOLCHAIN_PATH}${PREFIX}cpp)


SET(OBJCOPY ${TOOLCHAIN_PATH}/arm-none-eabi-objcopy)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

