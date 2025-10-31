set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)


set(EMBY_STM32_BAREMETAL_PLATFORM ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "including cmake from: " ${EMBY_STM32_BAREMETAL_PLATFORM})

#override embyplatform config with folder name
EMBY_UTILS_GET_FOLDER_NAME(${EMBY_STM32_BAREMETAL_PLATFORM} EMBY_PLATFORM)
message(STATUS "EMBY PLATFORM: ${EMBY_PLATFORM}")

set(ENV{EMBY_CROSS_BUILD_ARCH} "ARM")
add_compile_definitions(EMBY_BUILD_ARM)
include("${EMBY_FOLDER}/toolchain.cmake")
enable_language(ASM)
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

if (${EMBY_PLATFORM}_DEVICE)
    add_definitions(-DUSE_HAL_DRIVER -D${${EMBY_PLATFORM}_DEVICE})
    message(STATUS "included device ${${EMBY_PLATFORM}_DEVICE}")
else()
    message(FATAL_ERROR "include device i.e, ${EMBY_PLATFORM}_DEVICE=STM32L431xx")
endif()

if (${EMBY_PLATFORM}_FAMILY)
    add_definitions(-D${${EMBY_PLATFORM}_FAMILY})
    message(STATUS "included family ${${EMBY_PLATFORM}_FAMILY}")
    include("${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/CMSIS/Device/ST/${${EMBY_PLATFORM}_FAMILY}/device.cmake")
else()
    message(FATAL_ERROR "include device i.e, ${EMBY_PLATFORM}_FAMILY=./device.cmake")
endif()

if (${EMBY_PLATFORM}_STARTUP)
    list(APPEND SOURCES ${${EMBY_PLATFORM}_STARTUP})
    message(STATUS "Startup file: ${${EMBY_PLATFORM}_STARTUP}")
else()
    message(FATAL_ERROR "include startup file, i.e ${EMBY_PLATFORM}_STARTUP=startup_stm32l431cctx.S")
endif()

if (${EMBY_PLATFORM}_CONF_DIR)
    message(STATUS "CONF DIRECTORY: ${${EMBY_PLATFORM}_CONF_DIR}")
    include_directories(${${EMBY_PLATFORM}_CONF_DIR})
else()
    message(FATAL_ERROR "include HAL conf file, i.e ${EMBY_PLATFORM}_CONF_DIR=./conf")
endif()

if (${EMBY_PLATFORM}_LINKER)
    set(LINKER_SCRIPT ${${EMBY_PLATFORM}_LINKER})
else()
    message(FATAL_ERROR "include Linker ld file, i.e ${EMBY_PLATFORM}_LINKER=STM32L431CCTX_FLASH.ld")
endif()

# remove emby root from include dirs
EMBY_UTILS_SYNC_IMPL_INCLUDES(${EMBY_STM32_BAREMETAL_PLATFORM})
#include all sbdirectories
include_directories(${EMBY_STM32_BAREMETAL_PLATFORM})
#EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS(${EMBY_STM32_BAREMETAL_PLATFORM} TRUE)


foreach(subdir ${SUBDIR})
    if(NOT ${subdir} MATCHES "^${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers")
        include_directories(${subdir})
        message(STATUS "include: ${subdir}")
    endif()
endforeach()

set(DRIVERS "")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/CMSIS/Include")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/CMSIS/Device/ST/${${EMBY_PLATFORM}_FAMILY}/Include")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/CMSIS/Device/ST/${${EMBY_PLATFORM}_FAMILY}/Include/Templates")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/hal_conf")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/hal_conf/${${EMBY_PLATFORM}_FAMILY}_HAL_Driver/Inc")
list(APPEND DRIVERS "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/hal_conf/${${EMBY_PLATFORM}_FAMILY}_HAL_Driver/Inc/Legacy")
foreach(subdir ${DRIVERS})
    include_directories(${subdir})
    message(STATUS "include: ${subdir}")
endforeach()

message(STATUS "include cmake from: ${EMBY_PLATFORM}")

#add extra platform sources
file(GLOB_RECURSE EMBY_PLAT_SOURCES
      "${EMBY_STM32_BAREMETAL_PLATFORM}/Startup/*/*.c"
    "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/CMSIS/*/*/${${EMBY_PLATFORM}_FAMILY}/*/*.c"
    "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/hal_conf/${${EMBY_PLATFORM}_FAMILY}_HAL_Driver/*/*.c"
)

#add common Emby platform sources
EMBY_UTILS_ADD_SOURCES(${EMBY_STM32_BAREMETAL_PLATFORM}  EMBY_PLAT_SOURCES )

list(APPEND EMBY_SOURCES ${EMBY_PLAT_SOURCES})

# add_link_options(-lc -lm -lnosys)
set(CMAKE_CXX_LINK_FLAGS "    -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs ${COMPILE_OPTS}" CACHE INTERNAL "Linker options")
set(CMAKE_C_LINK_FLAGS "      -Wl,--gc-sections --specs=nano.specs --specs=nosys.specs ${COMPILE_OPTS}" CACHE INTERNAL "Linker options")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")
# add_compile_options(-nostdlib)


list(APPEND SOURCES ${EMBY_SOURCES})


