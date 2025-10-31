
if (DEFINED ENV{FREERTOS_PATH} AND (NOT FREERTOS_PATH))
    set(FREERTOS_PATH $ENV{FREERTOS_PATH})
    message("Using FREERTOS_PATH from environment ('${FREERTOS_PATH}')")
endif ()


if (DEFINED ENV{FREERTOS_CONFIG_FILE_DIRECTORY} AND (NOT FREERTOS_CONFIG_FILE_DIRECTORY))
    set(FREERTOS_CONFIG_FILE_DIRECTORY "$ENV{FREERTOS_CONFIG_FILE_DIRECTORY}")
    message("Using FREERTOS_CONFIG_FILE_DIRECTORY from environment ('${FREERTOS_CONFIG_FILE_DIRECTORY}')")
endif ()

if (DEFINED ENV{FREERTOS_PORTABLE_PATH} AND (NOT FREERTOS_PORTABLE_PATH))
    set(FREERTOS_PORTABLE_PATH "$ENV{FREERTOS_PORTABLE_PATH}")
    message("Using FREERTOS_PORTABLE_PATH from environment ('${FREERTOS_PORTABLE_PATH}')")
endif ()



if (NOT FREERTOS_PATH)
    message(FATAL_ERROR "FreeRTOS location was not specified. Please set FREERTOS_PATH.")
endif()

if (NOT FREERTOS_PORTABLE_PATH)
    message(FATAL_ERROR "FreeRTOS location was not specified. Please set FREERTOS_PORTABLE_PATH.")
endif()


set(FREERTOS_PATH "${FREERTOS_PATH}" CACHE PATH "Path to the FreeRTOS Kernel")


# Heap number or absolute path to custom heap implementation provided by user
set(FREERTOS_HEAP "4" CACHE STRING "FreeRTOS heap model number. 1 .. 5. Or absolute path to custom heap source file")

message(STATUS " FREERTOS DIR:   ${FREERTOS_PATH} ")
message(STATUS " FREERTOS PORTABLE DIR:   ${FREERTOS_PORTABLE_PATH} ")

include_directories(${FREERTOS_PATH}/include/)
file(GLOB_RECURSE FREERTOS_SOURCES
        ${FREERTOS_PATH}/croutine.c
        ${FREERTOS_PATH}/event_groups.c
        ${FREERTOS_PATH}/list.c
        ${FREERTOS_PATH}/queue.c
        ${FREERTOS_PATH}/stream_buffer.c
        ${FREERTOS_PATH}/tasks.c
        ${FREERTOS_PATH}/timers.c
        ${FREERTOS_PATH}/portable/MemMang/heap_${FREERTOS_HEAP}.c
)

#message(STATUS $<IF:$<BOOL:$<FILTER:${FREERTOS_HEAP},EXCLUDE,^[1-5]$>>,${FREERTOS_HEAP},${FREERTOS_PATH}/portable/MemMang/heap_${FREERTOS_HEAP}.c>)

message(STATUS ${FREERTOS_PATH}/portable/MemMang/heap_${FREERTOS_HEAP}.c)
#add_definitions(-include "${FREERTOS_CONFIG_FILE_DIRECTORY}/FreeRTOSConfig.h")
include_directories(${FREERTOS_PORTABLE_PATH})
include_directories(${FREERTOS_CONFIG_FILE_DIRECTORY})

file(GLOB_RECURSE FREERTOS_PORTABLE_SOURCES
        ${FREERTOS_PORTABLE_PATH}/*.c)

file(GLOB_RECURSE FREERTOS_WRAPPER_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/OsHook.cc
        ${CMAKE_CURRENT_LIST_DIR}/CppWrapper.cc)

list(APPEND SOURCES ${FREERTOS_SOURCES})
list(APPEND SOURCES ${FREERTOS_PORTABLE_SOURCES})
list(APPEND SOURCES ${FREERTOS_WRAPPER_SOURCES})

add_compile_definitions(FREERTOS)
