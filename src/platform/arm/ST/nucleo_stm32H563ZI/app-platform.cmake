# Set the system name and version
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Specify the minimum required CMake version
cmake_minimum_required(VERSION 3.17)

# Display a status message
message(STATUS "app-platform.cmake for H5..")

# Include directories
include_directories(
    "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H563ZI"
    "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H563ZI/conf"
)

# Gather application source files
file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H563ZI/EmbyAppMachine.cc"
    "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H563ZI/system_stm32h5xx.c"
    "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H563ZI/stm32h5xx_it.c"
)

# RTOS selection logic (commented out)
# if (NOT DEFINED EMBY_RTOS OR ("${EMBY_RTOS}" STREQUAL ""))
#     message(STATUS "NO RTOS Support!")
#     ####FREERTOS
# elseif (${EMBY_RTOS} STREQUAL "freertos")
#     file(GLOB_RECURSE FREERTOS_WRAPPER_SOURCES
#         "${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/*.cc"
#     )
#     list(APPEND SOURCES ${FREERTOS_WRAPPER_SOURCES})
#
#     set(ENV{FREERTOS_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/FreeRTOS-KernelV10.6.2)
#     set(ENV{FREERTOS_CONFIG_FILE_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/config/)
#     set(ENV{FREERTOS_PORTABLE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/FreeRTOS-KernelV10.6.2/portable/GCC/ARM_CM33_NTZ/non_secure/)
#     include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/freertos-kernel.cmake")
#
#     # STACK-TCP
#     # set(ENV{FREERTOS_TCP_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/stackIP/source)
#     # set(ENV{FREERTOS_TCP_CONFIG_FILE_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/stackIP)
#     # set(ENV{FREERTOS_PORTABLE_TCP_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/stackIP/source/portable/NetworkInterface/STM32N6xx)
#     # include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/FreeRTOS/freertos-tcp.cmake")
#     ######################### LWIP
#     # set(ENV{LWIP_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/StackIP/lwip-STABLE-2_2_0)
#     # set(ENV{LWIP_CONFIG_FILE_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/StackIP/conf)
#     # include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/StackIP/lwip.cmake")
#
# elseif (${EMBY_RTOS} STREQUAL "threadx")
#
#     set(ENV{THREADX_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/threadx)
#     set(ENV{THREADX_PORTABLE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/threadx/ports/cortex_m33/gnu)
#     include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/threadx.cmake")
#
#     # Then the common files
#     set(THREADX_ARCH cortex_m33)
#     set(THREADX_TOOLCHAIN gnu)
#     set(ENV{NETXDUO_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/netxduo)
#     set(ENV{NETXDUO_PORTABLE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/netxduo/ports/cortex_m33/gnu)
#     include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/threadx.cmake")
#
#     include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/conf")
#     set(NX_USER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/conf/nx_user.h)
#     # include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/netxduo/common/inc")
#     # include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/netxduo/ports/${THREADX_ARCH}/${THREADX_TOOLCHAIN}/inc")
#     # add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/netxduo)
#
#     include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/netxduo.cmake")
#     # add eth phy drivers
#     include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/Threadx/drivers/netxduo-drivers.cmake")
#
#     ## LWIP
#     # set(ENV{LWIP_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/StackIP/lwip-STABLE-2_2_0)
#     # set(ENV{LWIP_CONFIG_FILE_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/StackIP/conf)
#     # include("${CMAKE_CURRENT_SOURCE_DIR}/src/arm/RTOS/StackIP/lwip.cmake")
#
# endif ()

# Append sources to the list
list(APPEND SOURCES ${APP_SOURCES} ${EMBY_SOURCES})

# Add linker options
add_link_options(-T ${LINKER_SCRIPT})
message(STATUS "LINKER: " ${LINKER_SCRIPT})

# add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
# target_link_libraries(${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/src/Audio/ai/lib/ARM/NetworkRuntime730_CM33_GCC.a)
