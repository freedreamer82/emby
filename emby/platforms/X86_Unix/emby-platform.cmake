set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(EMBY_X86_PLATFORM ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "including cmake from: " ${EMBY_X86_PLATFORM})


set(ENV{EMBY_CROSS_BUILD_ARCH} "X86")
add_compile_definitions(EMBY_BUILD_X86)

# remove emby root from include dirs
EMBY_UTILS_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(${EMBY_X86_PLATFORM})

#include all sbdirectories
include_directories(${EMBY_X86_PLATFORM})

EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS(${EMBY_X86_PLATFORM} TRUE)

#Add sources cc,c
EMBY_UTILS_ADD_SOURCES(${EMBY_X86_PLATFORM} EMBY_PLAT_SOURCES)

list(APPEND SOURCES ${EMBY_PLAT_SOURCES})

# Crea la libreria core con suffisso della piattaforma (es: emby_core_X86_Unix) e alias emby_core
EMBY_UTILS_CREATE_NAMED_LIB(emby_core ${EMBY_X86_PLATFORM} ${EMBY_FOLDER}/core EMBY_SOURCES)

# Crea la libreria specifica per la piattaforma (es: emby_platform_X86_Unix) e alias emby_platform
EMBY_UTILS_CREATE_NAMED_LIB(emby_platform ${EMBY_X86_PLATFORM} ${EMBY_X86_PLATFORM} EMBY_PLAT_SOURCES)

# Collegamento: fai sì che la libreria di piattaforma dipenda dalla libreria core specifica
get_filename_component(_emby_suffix_basename ${EMBY_X86_PLATFORM} NAME)
string(REGEX REPLACE "[^A-Za-z0-9]" "_" _emby_suffix_basename ${_emby_suffix_basename})
set(_emby_core_target_name "emby_core_${_emby_suffix_basename}")
set(_emby_platform_target_name "emby_platform_${_emby_suffix_basename}")
if(TARGET ${_emby_platform_target_name} AND TARGET ${_emby_core_target_name})
    target_link_libraries(${_emby_platform_target_name} PRIVATE ${_emby_core_target_name})
    message(STATUS "Linked ${_emby_platform_target_name} -> ${_emby_core_target_name}")
endif()

EMBY_UTILS_ADD_TESTS(${EMBY_X86_PLATFORM})
