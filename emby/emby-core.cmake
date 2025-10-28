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


# Macro per raccogliere i sorgenti core Emby a partire da una root
macro(EMBY_ADD_SOURCES START_ROOT OUT_VAR)
    # Moduli core da includere
    set(_emby_core_modules
        EmbyConsole
        EmbyDebug
        EmbyLibs
        EmbyLog
        EmbyMachine
        EmbySystem
        EmbyThreading
        EmbyTime
    )

    set(_emby_list "")
    foreach(mod ${_emby_core_modules})
        # cerca .cc ricorsivamente nella directory del modulo
        file(GLOB_RECURSE _mod_sources "${START_ROOT}/${mod}/*.cc" "${START_ROOT}/${mod}/*.c")
        list(APPEND _emby_list ${_mod_sources})
    endforeach()

    # Rimuovi duplicati nella lista appena trovata
    list(REMOVE_DUPLICATES _emby_list)

    # Append alla variabile di output esistente, poi rimuovi duplicati finali
    set(_emby_existing "${${OUT_VAR}}")
    set(_emby_new ${_emby_existing} ${_emby_list})
    list(REMOVE_DUPLICATES _emby_new)
    # Non usare PARENT_SCOPE: macro esegue nel contesto della chiamata, quindi impostare direttamente
    set(${OUT_VAR} ${_emby_new})
endmacro()


# Macro per aggiungere test: per ogni sottocartella in START_ROOT crea un eseguibile se sono presenti file Test_*.cc o altri .cc
macro(EMBY_ADD_TESTS START_ROOT)
    # Preferisci la directory START_ROOT/EmbyTest se esiste, altrimenti se START_ROOT è già EmbyTest usala
    if(EXISTS "${START_ROOT}/EmbyTest")
        set(TEST_ROOT "${START_ROOT}/EmbyTest")
    else()
        # Controlla se START_ROOT termina con EmbyTest
        string(REGEX MATCH "EmbyTest$" _is_embytest "${START_ROOT}")
        if(_is_embytest)
            set(TEST_ROOT "${START_ROOT}")
        else()
            message(STATUS "EMBY_ADD_TESTS: nessuna directory EmbyTest trovata in ${START_ROOT}; skipping tests generation")
            return()
        endif()
    endif()

    # Ottieni le entry immediate nella directory TEST_ROOT
    file(GLOB children RELATIVE ${TEST_ROOT} ${TEST_ROOT}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${TEST_ROOT}/${child})
            # Cerca file di test nella sottodirectory
            file(GLOB test_sources "${TEST_ROOT}/${child}/Test_*.cc" "${TEST_ROOT}/${child}/*.cc")
            if(test_sources)
                # Normalizza il nome della sottocartella per creare un nome target valido
                get_filename_component(child_name ${child} NAME)
                string(REPLACE "/" "_" child_name_sanitized ${child_name})
                string(REGEX REPLACE "[^A-Za-z0-9_]" "_" child_name_sanitized ${child_name_sanitized})

                set(target_name "Test_${child_name_sanitized}")

                # Crea l'eseguibile usando i sorgenti di test
                add_executable(${target_name} ${test_sources})

                # Includi cartelle core e la directory del test per gli header locali
                if(DEFINED EMBY_FOLDER)
                    target_include_directories(${target_name} PRIVATE ${EMBY_FOLDER}/core)
                endif()
                target_include_directories(${target_name} PRIVATE ${TEST_ROOT}/${child} ${EMBY_FOLDER})

                # Linka le librerie emby_core e emby_platform se esistono
                if(TARGET emby_core)
                    target_link_libraries(${target_name} PRIVATE emby_core)
                endif()
                if(TARGET emby_platform)
                    target_link_libraries(${target_name} PRIVATE emby_platform)
                endif()

                # Posiziona gli eseguibili di test in una cartella dedicata nella build
                set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

                message(STATUS "EMBY_ADD_TESTS: creato target ${target_name} con sorgenti: ${test_sources}")
            endif()
        endif()
    endforeach()
endmacro()


EMBY_ADD_SOURCES(${EMBY_FOLDER}/core  EMBY_SOURCES)

# RIMOSSA creazione diretta di emby_core qui: la creazione verrà effettuata dopo che la piattaforma è nota
# (prima si provava a creare emby_core senza suffisso platform, ora lo facciamo per piattaforma)

# Macro generica per creare una libreria con prefisso e suffisso della cartella di platform
# Uso: EMBY_CREATE_NAMED_LIB(<lib_prefix> <suffix_dir> <include_dir> <sources_variable_name>)
# Esempio per platform: EMBY_CREATE_NAMED_LIB(emby_platform ${EMBY_X86_PLATFORM} ${EMBY_X86_PLATFORM} EMBY_PLAT_SOURCES)
# Esempio per core (creato nella fase di platform): EMBY_CREATE_NAMED_LIB(emby_core ${EMBY_X86_PLATFORM} ${EMBY_FOLDER}/core EMBY_SOURCES)
macro(EMBY_CREATE_NAMED_LIB LIB_PREFIX SUFFIX_DIR INCLUDE_DIR SOURCES_VAR)
    # verifica che la variabile SOURCES_VAR non sia vuota (evita espansione multipla in if)
    if(NOT "${${SOURCES_VAR}}" STREQUAL "")
        # Ottieni il nome della cartella usata per il suffisso
        get_filename_component(_emby_suffix_basename ${SUFFIX_DIR} NAME)
        # Sanitize: sostituisci caratteri non alfanumerici con underscore
        string(REGEX REPLACE "[^A-Za-z0-9]" "_" _emby_suffix_basename ${_emby_suffix_basename})

        # Nome target specifico
        set(_emby_lib_target "${LIB_PREFIX}_${_emby_suffix_basename}")

        # Crea il target specifico
        add_library(${_emby_lib_target} STATIC ${${SOURCES_VAR}})
        target_include_directories(${_emby_lib_target} PUBLIC ${INCLUDE_DIR})
        message(STATUS "Created library target: ${_emby_lib_target} (from ${SOURCES_VAR})")

        # Crea un alias generico <LIB_PREFIX> per compatibilità con il codice esistente
        if(NOT TARGET ${LIB_PREFIX})
            add_library(${LIB_PREFIX} ALIAS ${_emby_lib_target})
        endif()
    else()
        message(STATUS "EMBY_CREATE_NAMED_LIB: variable ${SOURCES_VAR} is empty; skipping lib creation for ${SUFFIX_DIR}")
    endif()
endmacro()

if (EMBY_CMAKE_APPEND)
         # GCC or Clang
         include("${EMBY_CMAKE_APPEND}")
         message(STATUS "Including custom cmake: " ${EMBY_CMAKE_APPEND})
endif()
