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
if(EMBY_FOLDER)
    message(STATUS "including EMBY cmake: ${EMBY_FOLDER}/emby.cmake")
    set(ENV{EMBY_FOLDER} ["${EMBY_FOLDER}"])
    include("${EMBY_FOLDER}/emby.cmake")
endif()

# Compiler options
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

# Include directories
include_directories(src/* src/*/*/)

if("${EMBY_PLATFORM}" STREQUAL "X86_Unix")
    message(STATUS "Including APP x86 PLATFORM")
    include("src/platform/x86/app-platform.cmake")
elseif("${EMBY_PLATFORM}" STREQUAL "STM32xx_Baremetal")
    if("${STM32xx_Baremetal_FAMILY}" STREQUAL "STM32H5xx")
        message(STATUS "Including APP H5 PLATFORM")
        include("src/platform/arm/ST/nucleo_stm32H563ZI/app-platform.cmake")
    endif()
endif()

# Append example project to SOURCES
file(GLOB_RECURSE APP_SOURCES
    "src/main.cc"
    "src/Console/ConsoleCommands.cc"
    "src/MainWorker.cc"
)

list(APPEND SOURCES ${APP_SOURCES} ${EMBY_SOURCES})

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

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

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
