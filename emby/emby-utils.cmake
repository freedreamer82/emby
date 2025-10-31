# emby-utils.cmake
# Macros estratte da emby-core.cmake per riuso e pulizia

macro(EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS root_folder debug_mode)
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


macro(EMBY_UTILS_SYNC_IMPL_INCLUDES platform_root)
    # Usa ARGV1 per accedere al secondo argomento opzionale
    set(core_root "${ARGV1}")

    message(STATUS "EMBY_UTILS: called with platform_root='${platform_root}' core_root='${core_root}'")

    # Determina quale core root usare: se core_root è passato ed esiste usalo,
    # altrimenti prova a usare EMBY_CORE_FOLDER se definita ed esistente.
    set(chosen_core_root "")
    if(NOT "${core_root}" STREQUAL "" AND EXISTS "${core_root}")
        set(chosen_core_root "${core_root}")
        message(STATUS "EMBY_UTILS: using passed core_root='${chosen_core_root}'")
    elseif(DEFINED EMBY_CORE_FOLDER AND NOT "${EMBY_CORE_FOLDER}" STREQUAL "" AND EXISTS "${EMBY_CORE_FOLDER}")
        set(chosen_core_root "${EMBY_CORE_FOLDER}")
        message(STATUS "EMBY_UTILS: using EMBY_CORE_FOLDER='${chosen_core_root}'")
    else()
        message(STATUS "EMBY_UTILS: no core root available; will skip removal from core roots")
    endif()

    # Aggiungo le include delle sottodirectory Impl della piattaforma in una sola chiamata
    # Usa il macro che include solo le sottodirectory Impl (debug-mode = FALSE per default non verboso)
    if(EXISTS "${platform_root}")
        EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS(${platform_root} TRUE)
    else()
        message(STATUS "EMBY_UTILS: platform_root '${platform_root}' does not exist; skipping include of Impl dirs")
    endif()

    file(GLOB plat_impl_dirs "${platform_root}/*/Impl")

    foreach(plat_impl_dir ${plat_impl_dirs})
        get_filename_component(parent_dir ${plat_impl_dir} DIRECTORY)
        get_filename_component(module_name ${parent_dir} NAME)
        file(GLOB plat_impl_subdirs "${plat_impl_dir}/*/")

        foreach(plat_impl_subdir ${plat_impl_subdirs})
            file(GLOB hh_files "${plat_impl_subdir}/*.hh")
            if(hh_files)
                get_filename_component(subdir_name ${plat_impl_subdir} NAME)

                # Rimuovi la corrispondente Impl nel chosen_core_root se disponibile
                if(NOT "${chosen_core_root}" STREQUAL "")
                    set(core_root_impl_subdir "${chosen_core_root}/${module_name}/Impl/${subdir_name}")
                    if(EXISTS ${core_root_impl_subdir})
                        # Usa la macro centralizzata per rimuovere dalle include globali
                        EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE(${core_root_impl_subdir})
                    endif()
                endif()

                # Se è stato passato un core_root esplicito diverso da EMBY_CORE_FOLDER,
                # rimuovi la corrispondente Impl anche da EMBY_CORE_FOLDER
                if(NOT "${core_root}" STREQUAL "" AND DEFINED EMBY_CORE_FOLDER AND NOT "${EMBY_CORE_FOLDER}" STREQUAL "" AND NOT "${core_root}" STREQUAL "${EMBY_CORE_FOLDER}")
                    set(core_impl_subdir "${EMBY_CORE_FOLDER}/${module_name}/Impl/${subdir_name}")
                    if(EXISTS ${core_impl_subdir})
                        EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE(${core_impl_subdir})
                    endif()
                endif()

            endif()
        endforeach()
    endforeach()
endmacro()


macro(EMBY_UTILS_PRINT_GLOBAL_IMPL_INCLUDES)
    # Ottieni le include globali del directory corrente
    get_property(_global_includes DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    if(NOT _global_includes)
        message(STATUS "EMBY_UTILS: no global include directories set")
        # Assicura che IMPL_INCLUDE_DIRS sia vuota nello scope corrente
        set(IMPL_INCLUDE_DIRS "")
        return()
    endif()

    set(_found_list "")
    foreach(_inc ${_global_includes})
        # Normalizza separatori (utile su Windows), e verifica se il path contiene il pattern Emby.../Impl
        string(REPLACE "\\\\" "/" _inc_norm "${_inc}")
        string(REGEX MATCH ".*Emby[^/]+/Impl(/.*)?$" _match "${_inc_norm}")
        if(_match)
            list(APPEND _found_list ${_inc})
        endif()
    endforeach()

    # Esponi la lista nella variabile IMPL_INCLUDE_DIRS nello scope corrente (macro eseguita nello scope chiamante)
    set(IMPL_INCLUDE_DIRS ${_found_list})

    # Stampa direttamente le directory trovate usando il costrutto richiesto
    foreach(_dir IN LISTS IMPL_INCLUDE_DIRS)
        message(STATUS "Included impl dir: ${_dir}")
    endforeach()
endmacro()


macro(EMBY_UTILS_INCLUDE_SUB_DIRS root_folder debug_mode)
    EMBY_UTILS_SUBDIRLIST(SUBDIRS "${root_folder}")
    foreach(subdir ${SUBDIRS})
        if(${debug_mode})
            message(STATUS "including dir: ${subdir}")
        endif()
        include_directories(${subdir})
    endforeach()
endmacro()


MACRO(EMBY_UTILS_SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/* ${curdir}/*/* ${curdir}/*/*/* ${curdir}/*/*/*/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${curdir}/${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()


# Macro per raccogliere i sorgenti core Emby a partire da una root
macro(EMBY_UTILS_ADD_SOURCES START_ROOT OUT_VAR)
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
macro(EMBY_UTILS_ADD_TESTS START_ROOT)
    # Preferisci la directory START_ROOT/EmbyTest se esiste, altrimenti se START_ROOT è già EmbyTest usala
    if(EXISTS "${START_ROOT}/EmbyTest")
        set(TEST_ROOT "${START_ROOT}/EmbyTest")
    else()
        # Controlla se START_ROOT termina con EmbyTest
        string(REGEX MATCH "EmbyTest$" _is_embytest "${START_ROOT}")
        if(_is_embytest)
            set(TEST_ROOT "${START_ROOT}")
        else()
            message(STATUS "EMBY_ADD_TESTS: no EmbyTest directory found in ${START_ROOT}; skipping tests generation")
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

                message(STATUS "EMBY_ADD_TESTS: created target ${target_name} with sources: ${test_sources}")
            endif()
        endif()
    endforeach()
endmacro()



macro(EMBY_UTILS_CREATE_NAMED_LIB LIB_PREFIX SUFFIX_DIR INCLUDE_DIR SOURCES_VAR)
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

MACRO(EMBY_UTILS_REMOVE_FILES_FROM_LIST_WITH_WILDCARD path listRemoveFrom)
    file(GLOB files ${path})
    foreach(file  ${files})
        message(STATUS "removing: " ${file})
        list(REMOVE_ITEM ${listRemoveFrom} ${file})
    endforeach()
ENDMACRO()



macro(EMBY_UTILS_PRINT_FILES_FROM_LIST_WITH_WILDCARD pattern list_var_name)
    if("${pattern}" STREQUAL "" OR "${list_var_name}" STREQUAL "")
        message(STATUS "EMBY_UTILS_PRINT_FILES_FROM_LIST_WITH_WILDCARD: pattern or list_var_name empty; nothing to do")
        return()
    endif()

    # candidate pattern resolutions (proviamo in ordine fino a che uno produce risultati)
    set(_candidates "")

    # se il pattern è già assoluto usa così com'è
    string(REGEX MATCH "^/" _is_abs "${pattern}")
    if(_is_abs)
        list(APPEND _candidates "${pattern}")
    else()
        # pattern non assoluto: proviamo diverse root
        # 1) come relativo alla CMAKE_CURRENT_SOURCE_DIR
        list(APPEND _candidates "${CMAKE_CURRENT_SOURCE_DIR}/${pattern}")
        # 2) come relativo alla CMAKE_SOURCE_DIR (root del progetto)
        if(DEFINED CMAKE_SOURCE_DIR)
            list(APPEND _candidates "${CMAKE_SOURCE_DIR}/${pattern}")
        endif()
        # 3) come relativo a EMBY_FOLDER se definita
        if(DEFINED EMBY_FOLDER)
            list(APPEND _candidates "${EMBY_FOLDER}/${pattern}")
        endif()
        # 4) come fornito (potrebbe essere un pattern con wildcard che funziona da solo)
        list(APPEND _candidates "${pattern}")
    endif()

    set(_matched_paths "")
    set(_used_candidate "")
    foreach(_cand ${_candidates})
        # file(GLOB_RECURSE) richiede il pattern tra virgolette
        file(GLOB_RECURSE _tmp_matched "${_cand}")
        if(_tmp_matched)
            set(_matched_paths ${_tmp_matched})
            set(_used_candidate ${_cand})
            break()
        endif()
    endforeach()

    if(NOT _matched_paths)
        message(STATUS "EMBY_UTILS: no path matches the pattern via glob: ${pattern}")
        message(STATUS "EMBY_UTILS: candidate patterns tried: ${_candidates}")
        # Fallback: proviamo a confrontare il pattern (glob) direttamente contro gli elementi della lista
        # Converti il pattern glob in regex (semplice): '*' -> '.*', '?' -> '.', escape altri metacaratteri
        set(_glob_pattern "${pattern}")
        # Se pattern relativo, normalizziamo come prima per migliori informazioni nello regex
        string(REGEX MATCH "^/" _is_abs_fallback "${_glob_pattern}")
        if(NOT _is_abs_fallback)
            set(_glob_pattern_rel "${CMAKE_CURRENT_SOURCE_DIR}/${_glob_pattern}")
        else()
            set(_glob_pattern_rel "${_glob_pattern}")
        endif()

        # Prepara regex: escape slash optional handling
        set(_regex "${_glob_pattern_rel}")
        # Escape regex special chars except * ?
        string(REGEX REPLACE "([\\.\+\^\$\(\)\[\]\{\}\|])" "\\\\\\1" _regex "${_regex}")
        string(REPLACE "*" ".*" _regex "${_regex}")
        string(REPLACE "?" "." _regex "${_regex}")

        # Ottieni gli elementi della lista passata
        set(_list_items2 "${${list_var_name}}")
        if(NOT _list_items2)
            message(STATUS "EMBY_UTILS: the list ${list_var_name} is empty or undefined (fallback)")
            return()
        endif()

        set(_found_any_fallback FALSE)
        foreach(_it ${_list_items2})
            string(REPLACE "\\\\" "/" _it_norm_fallback "${_it}")
            string(REGEX REPLACE "/+$" "" _it_norm_fallback "${_it_norm_fallback}")
            # matcha con regex (case sensitive)
            string(REGEX MATCH "${_regex}" _m "${_it_norm_fallback}")
            if(_m)
                message(STATUS "EMBY_UTILS (fallback): found in ${list_var_name}: ${_it}")
                set(_found_any_fallback TRUE)
            endif()
        endforeach()

        if(_found_any_fallback)
            return()
        else()
            message(STATUS "EMBY_UTILS: no matches even with fallback for pattern: ${pattern}")
            return()

        endif()
    endif()

    message(STATUS "EMBY_UTILS: pattern '${pattern}' resolved using: ${_used_candidate}")

    # Ottieni gli elementi della lista passata
    set(_list_items "${${list_var_name}}")
    if(NOT _list_items)
        message(STATUS "EMBY_UTILS: the list ${list_var_name} is empty or undefined")
        return()
    endif()

    set(_found_any FALSE)
    foreach(_p ${_matched_paths})
        # Normalizza separatori e rimuovi slash finali
        string(REPLACE "\\\\" "/" _p_norm "${_p}")
        string(REGEX REPLACE "/+$" "" _p_norm "${_p_norm}")
        foreach(_it ${_list_items})
            string(REPLACE "\\\\" "/" _it_norm "${_it}")
            string(REGEX REPLACE "/+$" "" _it_norm "${_it_norm}")

            # confronti: esatto o sottostringa
            string(FIND "${_it_norm}" "${_p_norm}" _find1)
            string(FIND "${_p_norm}" "${_it_norm}" _find2)
            if(_it_norm STREQUAL _p_norm OR NOT _find1 EQUAL -1 OR NOT _find2 EQUAL -1)
                message(STATUS "EMBY_UTILS: found in ${list_var_name}: ${_it}")
                set(_found_any TRUE)
            endif()
        endforeach()
    endforeach()

    if(NOT _found_any)
        message(STATUS "EMBY_UTILS: no matches found between resolved paths and list ${list_var_name}")
        message(STATUS "EMBY_UTILS: sample resolved path: ${_matched_paths}" )
        # stampa anche prima entry della lista per debug
        list(GET _list_items 0 _first_item)
        message(STATUS "EMBY_UTILS: sample list item: ${_first_item}")
    endif()
endmacro()



macro(EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE folder2remove)
    # get Include dir and remove inclusion (robusto rispetto a slash finali e separatori)
    if("${folder2remove}" STREQUAL "")
        return()
    endif()
    # Normalizza separatori
    string(REPLACE "\\\\" "/" _f_norm "${folder2remove}")
    # Rimuovi eventuale slash finale/assenza
    string(REGEX REPLACE "/+$" "" _f_no_slash "${_f_norm}")
    set(_variants "${_f_no_slash}" "${_f_no_slash}/")

    # Leggi include della DIRECTORY corrente
    get_property(dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    if(NOT dirs)
        message(STATUS "EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE: no include directories set; nothing to remove")
        return()
    endif()

    set(_removed_any FALSE)

    # Per ogni variante normalizzata, confrontiamo con le entry correnti normalizzando anch'esse
    foreach(_variant ${_variants})
        # normalizza la variante per confronti (rimuove backslash e slash finali)
        string(REPLACE "\\\\" "/" _variant_norm "${_variant}")
        string(REGEX REPLACE "/+$" "" _variant_norm "${_variant_norm}")

        # controlla ciascuna entry esistente
        foreach(_entry ${dirs})
            string(REPLACE "\\\\" "/" _entry_norm "${_entry}")
            string(REGEX REPLACE "/+$" "" _entry_norm "${_entry_norm}")

            if(_entry_norm STREQUAL _variant_norm)
                list(REMOVE_ITEM dirs ${_entry})
                message(STATUS "removed include: ${_entry}")
                set(_removed_any TRUE)
            endif()
        endforeach()
    endforeach()

    # Rimuovi eventuali duplicati residui e aggiorna la proprietà
    list(REMOVE_DUPLICATES dirs)
    set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES ${dirs})

ENDMACRO()



# Macro alternativa per contesto locale
macro(EMBY_UTILS_GET_FOLDER_NAME path out_var)
    get_filename_component(_emby_folder_name "${path}" NAME)
    set(${out_var} "${_emby_folder_name}")
endmacro()
