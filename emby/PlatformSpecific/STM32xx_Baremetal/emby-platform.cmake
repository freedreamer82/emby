set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)


# Enable assembler files preprocessing
#add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)
# -mcmse to enable Cortex-M Security Extensions (CMSE) , -mfp16-format=ieee  for hal precision 16bit
#set(COMPILE_OPTS -mcpu=Cortex-M55 -mfpu=fpv5-d16 -mthumb -Wall   -mfloat-abi=hard -mcmse)
#add_compile_options("${COMPILE_OPTS}")
#add_link_options("${COMPILE_OPTS}")
#set(CMAKE_EXE_LINKER_FLAGS  "${COMPILE_OPTS}")


set(ENV{EMBY_CROSS_BUILD_ARCH} "ARM")
add_compile_definitions(EMBY_BUILD_ARM)
include("${EMBY_FOLDER}/toolchain.cmake")
enable_language(ASM)
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")


if (${EMBY_PLATFORM}_DEVICE)
    add_definitions(-DUSE_HAL_DRIVER -D${${EMBY_PLATFORM}_DEVICE})
    message(STATUS "included device " ${${EMBY_PLATFORM}_DEVICE})
else()
    message(FATAL_ERROR "include device i.e,${EMBY_PLATFORM}_DEVICE=STM32L431xx")
endif()


if (${EMBY_PLATFORM}_FAMILY)
    add_definitions(-D${${EMBY_PLATFORM}_FAMILY})
    message(STATUS "included family " ${${EMBY_PLATFORM}_FAMILY})
    include("${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/Device/ST/${${EMBY_PLATFORM}_FAMILY}/device.cmake")
else()
    message(FATAL_ERROR "include device i.e,${EMBY_PLATFORM}_FAMILY=./device.cmake")
endif()

if (${EMBY_PLATFORM}_STARTUP)
    list(APPEND SOURCES ${${EMBY_PLATFORM}_STARTUP})
    message(STATUS "Startup file: " ${${EMBY_PLATFORM}_STARTUP})
else()
    message(FATAL_ERROR "include statup file, i.e ${EMBY_PLATFORM}_STARTUP=startup_stm32l431cctx.S")
endif()


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



#include all sbdirectories
SUBDIRLIST(SUBDIR "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}")
foreach(subdir ${SUBDIR})
    if(NOT ${subdir} MATCHES "^${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers")
        include_directories(${subdir})
        message(STATUS "include    : " ${subdir})
    endif()
endforeach()

set(DRIVERS "")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/Include")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/Device/ST/${${EMBY_PLATFORM}_FAMILY}/Include")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/Device/ST/${${EMBY_PLATFORM}_FAMILY}/Include/Templates")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/hal_conf")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/hal_conf/${${EMBY_PLATFORM}_FAMILY}_HAL_Driver/Inc")
list(APPEND DRIVERS "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/hal_conf/${${EMBY_PLATFORM}_FAMILY}_HAL_Driver/Inc/Legacy")
foreach(subdir ${DRIVERS})
    include_directories(${subdir})
    message(STATUS "include    : " ${subdir})
endforeach()


message(STATUS "include cmake from: " ${EMBY_PLATFORM})

file(GLOB_RECURSE EMBY_PLAT_SOURCES
        "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Emby*/*.cc"
        "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Startup/*/*.c"
        "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/CMSIS/*/*/${${EMBY_PLATFORM}_FAMILY}/*/*.c"
        "${EMBY_FOLDER}/PlatformSpecific/${EMBY_PLATFORM}/Drivers/hal_conf/${${EMBY_PLATFORM}_FAMILY}_HAL_Driver/*/*.c")

list(APPEND EMBY_SOURCES ${EMBY_PLAT_SOURCES})


#add_link_options(  -lc -lm -lnosys  )
set(CMAKE_CXX_LINK_FLAGS "    -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs ${COMPILE_OPTS}  " CACHE INTERNAL "Linker options")
set(CMAKE_C_LINK_FLAGS "      -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs ${COMPILE_OPTS}  " CACHE INTERNAL "Linker options")
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
#add_compile_options(-nostdlib)
