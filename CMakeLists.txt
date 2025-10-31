cmake_minimum_required(VERSION 3.17)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Platform-specific linker flags
if(APPLE)
    # macOS specific settings
else()
    set(LDFLAGS "${LDFLAGS} --gc-sections")
endif()

# Include Emby framework

include("emby/emby-core.cmake")

# Compiler options
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# Include directories
include_directories(src/* src/*/*/)

message(STATUS "PLATFORM: " ${EMBY_PLATFORM})

if("${EMBY_PLATFORM}" STREQUAL "X86_Unix")
    message(STATUS "Including APP x86 PLATFORM")
    include("emby/platforms/X86_Unix/emby-platform.cmake")
    include("src/platform/x86/app-platform.cmake")

elseif("${EMBY_PLATFORM}" STREQUAL "STM32xx_Baremetal")
    if("${STM32xx_Baremetal_FAMILY}" STREQUAL "STM32H5xx")
        message(STATUS "Including APP H5 PLATFORM baremetal")
        include("emby/platforms/STM32xx_Baremetal/emby-platform.cmake")
        include("src/platform/arm/ST/nucleo_stm32H563ZI/app-platform.cmake")
    elseif ("${STM32xx_Baremetal_FAMILY}" STREQUAL "STM32H7xx")
        message(STATUS "Including APP H7 PLATFORM baremetal")
        include("emby/platforms/STM32xx_Baremetal/emby-platform.cmake")
        include("src/platform/arm/ST/nucleo_stm32H743ZI/app-platform.cmake")
    endif()
elseif("${EMBY_PLATFORM}" STREQUAL "STM32xx_FreeRTOS")
    if("${STM32xx_Baremetal_FAMILY}" STREQUAL "STM32H5xx")
        message(STATUS "Including APP STM32H5 FreeRTOS PLATFORM")
        #set FreeRTOS environment variables for stm32h5 m33
        set(ENV{FREERTOS_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/emby/platforms/RTOS/FreeRTOS/FreeRTOS-KernelV10.6.2)
        set(ENV{FREERTOS_CONFIG_FILE_DIRECTORY} ${CMAKE_CURRENT_SOURCE_DIR}/emby/platforms/RTOS/FreeRTOS/conf)
        set(ENV{FREERTOS_PORTABLE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/emby/platforms/RTOS/FreeRTOS/FreeRTOS-KernelV10.6.2/portable/GCC/ARM_CM33_NTZ/non_secure/)

        include("emby/platforms/STM32xx_FreeRTOS/emby-platform.cmake")
        include("src/platform/arm/ST/nucleo_stm32H563ZI/app-platform.cmake")

    elseif ("${STM32xx_Baremetal_FAMILY}" STREQUAL "STM32H7xx")
    endif ()
endif()

# Append example project to SOURCES
file(GLOB_RECURSE APP_SOURCES
    "src/main.cc"
    "src/Console/ConsoleCommands.cc"
    "src/MainWorker.cc"
)

# Non aggiungere più EMBY_SOURCES direttamente a SOURCES: useremo librerie
list(APPEND SOURCES ${APP_SOURCES})

# NOTE: emby_core and emby_platform library targets are created in their respective
# cmake files (emby-core.cmake creates emby_core; platform cmake creates emby_platform)

# Project settings
project(emby-test_${EMBY_PLATFORM} CXX C ASM)


if(LINKER_SCRIPT)
    add_link_options(-T ${LINKER_SCRIPT})
    message(STATUS "LINKER: ${LINKER_SCRIPT}")

    if(LINUX)
        add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
    elseif(APPLE)
        add_link_options(-Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map)
    endif()

    add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
else()
    if(LINUX)
        add_link_options(-Wl,-gc-sections,--print-memory-usage)
    elseif(APPLE)
        add_link_options(-Wl)
        add_compile_options(-Werror=format)
    endif()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# Link main executable with emby libraries if they exist
if(TARGET emby_core)
    target_link_libraries(${PROJECT_NAME} PRIVATE emby_core)
endif()
if(TARGET emby_platform)
    target_link_libraries(${PROJECT_NAME} PRIVATE emby_platform)
endif()


if (${EMBY_PLATFORM} STREQUAL "STM32xx_Baremetal")
    set(CUSTOM_NAME_EXEC "emby-test_${EMBY_PLATFORM}_${STM32xx_Baremetal_DEVICE}")
endif ()

if (CUSTOM_NAME_EXEC)
    message(STATUS "change exec name in: " ${CUSTOM_NAME_EXEC})
    set_target_properties(${PROJECT_NAME}.elf PROPERTIES OUTPUT_NAME "${CUSTOM_NAME_EXEC}.elf")
    set(HEX_FILE ${PROJECT_BINARY_DIR}/${CUSTOM_NAME_EXEC}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${CUSTOM_NAME_EXEC}.bin)
else()
    set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
    set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
endif ()




# Generate hex and binary files
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf")
    add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} Building ${BIN_FILE}"
    )
endif()

# Report size function
function(show_object_size target)
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf")
        string(REPLACE "objcopy" "size" CMAKE_OBJSIZE "${CMAKE_OBJCOPY}")
        message(STATUS "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.elf}")
        message(STATUS "${CMAKE_OBJSIZE}")

        add_custom_command(
            TARGET ${target} POST_BUILD
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMAND ${CMAKE_OBJSIZE} --format=berkeley --totals ${CMAKE_CURRENT_BINARY_DIR}/${target}
        )
    endif()
endfunction()

# Show object size
show_object_size(${PROJECT_NAME}.elf)
