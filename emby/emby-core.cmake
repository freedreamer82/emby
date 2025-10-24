set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(EMBY_FOLDER ${CMAKE_CURRENT_LIST_DIR})
set(ENV{EMBY_FOLDER} ["${EMBY_FOLDER}"])

message(STATUS "Emby Core cmake included from: " ${EMBY_FOLDER})

macro(EMBY_INCLUDE_ONLY_IMPL_DIRS root_folder debug_mode)
    file(GLOB impl_dirs "${root_folder}/*/Impl")
    foreach(impl_dir ${impl_dirs})
        # Include le sottodirectory dentro Impl
        file(GLOB impl_subdirs "${impl_dir}/*/")
        foreach(impl_subdir ${impl_subdirs})
            if(${debug_mode})
                message(STATUS "including impl subdir: ${impl_subdir}")
            endif()
            include_directories(${impl_subdir})
        endforeach()
    endforeach()
endmacro()

#macro(EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT platform_root)
#    get_property(current_includes DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#
#    # Trova le directory Impl che esistono nella platform
#    file(GLOB plat_impl_dirs "${platform_root}/*/Impl/*/")
#
#    foreach(plat_impl_dir ${plat_impl_dirs})
#        # Ottieni il nome del modulo (es: EmbySystem da .../EmbySystem/Impl)
#        get_filename_component(parent_dir ${plat_impl_dir} DIRECTORY)
#        get_filename_component(module_name ${parent_dir} NAME)
#
#        # Costruisci il path corrispondente in core
#        set(core_impl "${EMBY_FOLDER}/core/${module_name}/Impl")
#
#        # Rimuovi solo se esiste in core
#        if(EXISTS ${core_impl})
#            list(REMOVE_ITEM current_includes ${core_impl})
#            message(STATUS "Rimossa include ovunque: ${core_impl}")
#        endif()
#    endforeach()
#
#    set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES ${current_includes})
#endmacro()


macro(EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT platform_root)
    get_property(current_includes DIRECTORY PROPERTY INCLUDE_DIRECTORIES)

    # Trova tutte le sottodirectory dentro Impl nella platform
    file(GLOB plat_impl_dirs "${platform_root}/*/Impl")

    foreach(plat_impl_dir ${plat_impl_dirs})
        # Ottieni il nome del modulo
        get_filename_component(parent_dir ${plat_impl_dir} DIRECTORY)
        get_filename_component(module_name ${parent_dir} NAME)

        # Trova le sottodirectory dentro Impl della platform
        file(GLOB plat_impl_subdirs "${plat_impl_dir}/*/")

        foreach(plat_impl_subdir ${plat_impl_subdirs})
            # Verifica se contiene file .hh
            file(GLOB hh_files "${plat_impl_subdir}/*.hh")
            if(hh_files)
                # Ottieni il nome della sottodirectory (es: Timer, Mutex)
                get_filename_component(subdir_name ${plat_impl_subdir} NAME)

                # Costruisci il path corrispondente in core
                set(core_impl_subdir "${EMBY_FOLDER}/core/${module_name}/Impl/${subdir_name}")

                # Rimuovi la sottodirectory specifica di core se esiste
                if(EXISTS ${core_impl_subdir})
                    list(REMOVE_ITEM current_includes ${core_impl_subdir})
                    message(STATUS "Platform provides ${module_name}/${subdir_name}; removed core include: ${core_impl_subdir}")
                endif()
            endif()
        endforeach()
    endforeach()

    set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES ${current_includes})
endmacro()




macro(EMBY_INCLUDE_SUB_DIRS root_folder debug_mode)
    SUBDIRLIST(SUBDIRS "${root_folder}")
    foreach(subdir ${SUBDIRS})
        if(${debug_mode})
            message(STATUS "including dir: ${subdir}")
        endif()
        include_directories(${subdir})
    endforeach()
endmacro()


MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/* ${curdir}/*/* ${curdir}/*/*/* ${curdir}/*/*/*/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${curdir}/${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

if (EMBY_CONFIG)
    if(MSVC)
        add_definitions(/FI"${EMBY_CONFIG}")
    else()
        # GCC or Clang
        message(STATUS "EMBY Config:" "${EMBY_CONFIG}")
        add_definitions(-include "${EMBY_CONFIG}")
    endif()
endif()


include("${EMBY_FOLDER}/toolchain.cmake")

add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)

if(LINUX)
  add_compile_options(-fsingle-precision-constant)
endif()

add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -Werror=return-type)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message(STATUS "Maximum optimization for size , no DEBUG")
    add_compile_options(-O0)
else ()
    message(STATUS "Minimal optimization, debug info included")
    add_compile_options(-Os -g)
    add_compile_definitions(DEBUG BUILD_DEBUG)
endif ()

message(STATUS "${EMBY_FOLDER}" /core/EmbyConfig)
include_directories(${EMBY_FOLDER}/core)
EMBY_INCLUDE_ONLY_IMPL_DIRS(${EMBY_FOLDER}/core TRUE)

file(GLOB_RECURSE EMBY_SOURCES
        "${EMBY_FOLDER}/core/EmbyConfig/*.cc"
        "${EMBY_FOLDER}/core/EmbyConsole/*.cc"
        "${EMBY_FOLDER}/core/EmbyDebug/*.cc"
        "${EMBY_FOLDER}/core/EmbyLibs/*.cc"
        "${EMBY_FOLDER}/core/EmbyLog/*.cc"
        "${EMBY_FOLDER}/core/EmbyMachine/*.cc"
        "${EMBY_FOLDER}/core/EmbySystem/*.cc"
        "${EMBY_FOLDER}/core/EmbyThreading/*.cc"
        "${EMBY_FOLDER}/core/EmbyTime/*.cc"
        )


list (APPEND EMBY_SOURCES ${EMBY_PLAT_SOURCES})


if (EMBY_CMAKE_APPEND)
         # GCC or Clang
         include("${EMBY_CMAKE_APPEND}")
         message(STATUS "Including custom cmake: " ${EMBY_CMAKE_APPEND})
endif()

