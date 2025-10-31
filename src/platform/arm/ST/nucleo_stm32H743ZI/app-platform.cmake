# Set the system name and version
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Specify the minimum required CMake version
cmake_minimum_required(VERSION 3.17)

# Display a status message
message(STATUS "app-platform.cmake for H7..")

# Include directories
include_directories(
        "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H743ZI"
        "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H743ZI/conf"
)

# Gather application source files
file(GLOB_RECURSE APP_SOURCES
        "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H743ZI/EmbyAppMachine.cc"
        "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H743ZI/system_stm32h7xx.c"
        "${CMAKE_SOURCE_DIR}/src/platform/arm/ST/nucleo_stm32H743ZI/stm32h7xx_it.c"
        )

add_compile_definitions(EMBY_BUILD_ARM EMBY_RTOS)

list(APPEND SOURCES ${APP_SOURCES} ${EMBY_SOURCES})

# Add linker options
add_link_options(-T ${LINKER_SCRIPT})
message(STATUS "LINKER: " ${LINKER_SCRIPT})

