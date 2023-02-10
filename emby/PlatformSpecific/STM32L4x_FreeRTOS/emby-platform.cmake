set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)


add_compile_definitions(ARM_MATH_CM4;ARM_MATH_MATRIX_CHECK;ARM_MATH_ROUNDING)
set(COMPILE_OPTS -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -mfloat-abi=hard )
add_compile_options("${COMPILE_OPTS}")
add_link_options("${COMPILE_OPTS}")
set(CMAKE_EXE_LINKER_FLAGS  "${COMPILE_OPTS}")

if (${EMBY_PLATFORM}_DEVICE)
    add_definitions(-DUSE_HAL_DRIVER -D${${EMBY_PLATFORM}_DEVICE})
    message(STATUS "included device " ${${EMBY_PLATFORM}_DEVICE})
else()
    message(FATAL_ERROR "include device i.e,${EMBY_PLATFORM}_DEVICE=STM32L431xx")
endif()

if (${EMBY_PLATFORM}_STARTUP)
    list(APPEND SOURCES ${${EMBY_PLATFORM}_STARTUP})
    message(STATUS "Startup file: " ${${EMBY_PLATFORM}_STARTUP})
else()
    message(FATAL_ERROR "include statup file, i.e ${EMBY_PLATFORM}_STARTUP=startup_stm32l431cctx.S")
endif()


#if (${EMBY_PLATFORM}_CONF_RTOS)
#    message(STATUS "FreeRTOS Conf: " ${${EMBY_PLATFORM}_CONF_RTOS})
#    include(${${EMBY_PLATFORM}_CONF_RTOS})
#else()
#    message(FATAL_ERROR "include FreeRTOS conf, i.e ${EMBY_PLATFORM}_CONF_RTOS=FreeRTOSConfig.h")
#endif()
#
#if (${EMBY_PLATFORM}_CONF_HAL)
#    message(STATUS "HAL Conf: " ${${EMBY_PLATFORM}_CONF_HAL})
#    include(${${EMBY_PLATFORM}_CONF_HAL})
#else()
#    message(FATAL_ERROR "include HAL conf file, i.e  ${EMBY_PLATFORM}_CONF_HAL=stm32l4xx_hal_conf.h")
#endif()

if (${EMBY_PLATFORM}_CONF_DIR)
    message(STATUS "CONF DIRECTORY: " ${${EMBY_PLATFORM}_CONF_DIR})
    include_directories(${${EMBY_PLATFORM}_CONF_DIR})
else()
    message(FATAL_ERROR "include HAL conf file, i.e  ${EMBY_PLATFORM}_CONF_DIR=./conf")
endif()


if (${EMBY_PLATFORM}_LINKER)
    set(LINKER_SCRIPT  ${${EMBY_PLATFORM}_LINKER})
else()
    message(FATAL_ERROR "include Linker ld file, i.e  ${EMBY_PLATFORM}_LINKER=STM32L431CCTX_FLASH.ld")
endif()



#include_directories( ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/conf)

include_directories( ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/STM32L4xx_HAL_Driver/Inc
        ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
        ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/FreeRTOS/Source/include
        ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/FreeRTOS/Source/CMSIS_RTOS_V2
        ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/FreeRTOS/Source/portable/GCC/ARM_CM4F
        ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/Device/ST/STM32L4xx/Include
        ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/Include)


set(ENV{EMBY_CROSS_BUILD_ARCH} "ARM")
add_compile_definitions(EMBY_BUILD_ARM)

message(STATUS "include cmake from: " ${EMBY_PLATFORM})

file(GLOB_RECURSE EMBY_PLAT_SOURCES
        "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*.cc"  "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*.c"  "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*/*.c ${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/*/*/*.cc")
list(APPEND SOURCES ${EMBY_PLAT_SOURCES})


add_link_options(  -lc -lm -lnosys  )
set(CMAKE_CXX_LINK_FLAGS "    -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs ${COMPILE_OPTS}  " CACHE INTERNAL "Linker options")
set(CMAKE_C_LINK_FLAGS "      -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs ${COMPILE_OPTS}  " CACHE INTERNAL "Linker options")