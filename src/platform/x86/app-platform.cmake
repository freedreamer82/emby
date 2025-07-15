# Set the system name and version
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Specify the minimum required CMake version
cmake_minimum_required(VERSION 3.17)

message(STATUS "app-platform.cmake for X86..")

add_compile_options(-fexceptions)

if (APPLE)
    # Apple-specific options (currently empty)
else ()
    set(LDFLAGS "${LDFLAGS} --gc-sections")
endif ()

include_directories(${CMAKE_SOURCE_DIR}/src/x86)

file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_SOURCE_DIR}/src/x86/EmbyAppMachine.cc"
)
list(APPEND SOURCES ${APP_SOURCES})

if (LINUX)
    add_link_options(-Wl,-gc-sections,--print-memory-usage)
elseif (APPLE)
    add_link_options(-Wl)
    add_compile_options(-Werror=format)
endif ()
