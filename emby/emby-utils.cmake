# emby-utils.cmake
# Macros extracted from emby-core.cmake for reuse and cleanup

macro(EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS root_folder debug_mode)
    file(GLOB impl_dirs "${root_folder}/*/Impl")
    foreach(impl_dir ${impl_dirs})
        # Include the subdirectories inside Impl
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
    # Use ARGV1 to access the second optional argument
    set(core_root "${ARGV1}")

    message(STATUS "EMBY_UTILS: called with platform_root='${platform_root}' core_root='${core_root}'")

    # Determine which core root to use: if core_root is passed and exists use it,
    # otherwise try EMBY_CORE_FOLDER if defined and exists.
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

    # Add include paths of the platform's Impl subdirectories in a single call
    # Use the macro that includes only Impl subdirectories (debug_mode = FALSE by default for non-verbose)
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

                # Remove the corresponding Impl in chosen_core_root if available
                if(NOT "${chosen_core_root}" STREQUAL "")
                    set(core_root_impl_subdir "${chosen_core_root}/${module_name}/Impl/${subdir_name}")
                    if(EXISTS ${core_root_impl_subdir})
                        # Use the centralized macro to remove from global includes
                        EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE(${core_root_impl_subdir})
                    endif()
                endif()

                # If an explicit core_root different from EMBY_CORE_FOLDER was passed,
                # remove the corresponding Impl also from EMBY_CORE_FOLDER
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
    # Get the global include directories of the current directory
    get_property(_global_includes DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    if(NOT _global_includes)
        message(STATUS "EMBY_UTILS: no global include directories set")
        # Ensure IMPL_INCLUDE_DIRS is empty in the current scope
        set(IMPL_INCLUDE_DIRS "")
        return()
    endif()

    set(_found_list "")
    foreach(_inc ${_global_includes})
        # Normalize separators (useful on Windows), and check if the path contains the pattern Emby.../Impl
        string(REPLACE "\\\\" "/" _inc_norm "${_inc}")
        string(REGEX MATCH ".*Emby[^/]+/Impl(/.*)?$" _match "${_inc_norm}")
        if(_match)
            list(APPEND _found_list ${_inc})
        endif()
    endforeach()

    # Expose the list in IMPL_INCLUDE_DIRS variable in the current scope (macro runs in caller scope)
    set(IMPL_INCLUDE_DIRS ${_found_list})

    # Print the found directories directly using the requested construct
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


# Macro to collect Emby core sources starting from a root
macro(EMBY_UTILS_ADD_SOURCES START_ROOT OUT_VAR)
    # Core modules to include
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
        # search for .cc recursively in the module directory
        file(GLOB_RECURSE _mod_sources "${START_ROOT}/${mod}/*.cc" "${START_ROOT}/${mod}/*.c")
        list(APPEND _emby_list ${_mod_sources})
    endforeach()

    # Remove duplicates in the list just found
    list(REMOVE_DUPLICATES _emby_list)

    # Append to the existing output variable, then remove final duplicates
    set(_emby_existing "${${OUT_VAR}}")
    set(_emby_new ${_emby_existing} ${_emby_list})
    list(REMOVE_DUPLICATES _emby_new)
    # Do not use PARENT_SCOPE: macro runs in the caller context, so set directly
    set(${OUT_VAR} ${_emby_new})
endmacro()



# Macro to add tests: for each subfolder in START_ROOT create an executable if Test_*.cc or other .cc files are present
macro(EMBY_UTILS_ADD_TESTS START_ROOT)
    # Prefer START_ROOT/EmbyTest dir if it exists, otherwise if START_ROOT is already EmbyTest use it
    if(EXISTS "${START_ROOT}/EmbyTest")
        set(TEST_ROOT "${START_ROOT}/EmbyTest")
    else()
        # Check if START_ROOT ends with EmbyTest
        string(REGEX MATCH "EmbyTest$" _is_embytest "${START_ROOT}")
        if(_is_embytest)
            set(TEST_ROOT "${START_ROOT}")
        else()
            message(STATUS "EMBY_ADD_TESTS: no EmbyTest directory found in ${START_ROOT}; skipping tests generation")
            return()
        endif()
    endif()

    # Get immediate entries in TEST_ROOT directory
    file(GLOB children RELATIVE ${TEST_ROOT} ${TEST_ROOT}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${TEST_ROOT}/${child})
            # Find test files in the subdirectory
            file(GLOB test_sources "${TEST_ROOT}/${child}/Test_*.cc" "${TEST_ROOT}/${child}/*.cc")
            if(test_sources)
                # Normalize subfolder name to create a valid target name
                get_filename_component(child_name ${child} NAME)
                string(REPLACE "/" "_" child_name_sanitized ${child_name})
                string(REGEX REPLACE "[^A-Za-z0-9_]" "_" child_name_sanitized ${child_name_sanitized})

                set(target_name "Test_${child_name_sanitized}")

                # Create the executable using the test sources
                add_executable(${target_name} ${test_sources})

                # Include core folders and the test directory for local headers
                if(DEFINED EMBY_FOLDER)
                    target_include_directories(${target_name} PRIVATE ${EMBY_FOLDER}/core)
                endif()
                target_include_directories(${target_name} PRIVATE ${TEST_ROOT}/${child} ${EMBY_FOLDER})

                # Link emby_core and emby_platform libraries if they exist
                if(TARGET emby_core)
                    target_link_libraries(${target_name} PRIVATE emby_core)
                endif()
                if(TARGET emby_platform)
                    target_link_libraries(${target_name} PRIVATE emby_platform)
                endif()

                # Place test executables in a dedicated folder in the build
                set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests)

                message(STATUS "EMBY_ADD_TESTS: created target ${target_name} with sources: ${test_sources}")
            endif()
        endif()
    endforeach()
endmacro()



macro(EMBY_UTILS_CREATE_NAMED_LIB LIB_PREFIX SUFFIX_DIR INCLUDE_DIR SOURCES_VAR)
    # verify SOURCES_VAR is not empty (avoids multiple expansion in if)
    if(NOT "${${SOURCES_VAR}}" STREQUAL "")
        # Get the folder name used for the suffix
        get_filename_component(_emby_suffix_basename ${SUFFIX_DIR} NAME)
        # Sanitize: replace non-alphanumeric characters with underscore
        string(REGEX REPLACE "[^A-Za-z0-9]" "_" _emby_suffix_basename ${_emby_suffix_basename})

        # Specific target name
        set(_emby_lib_target "${LIB_PREFIX}_${_emby_suffix_basename}")

        # Create the specific target
        add_library(${_emby_lib_target} STATIC ${${SOURCES_VAR}})
        target_include_directories(${_emby_lib_target} PUBLIC ${INCLUDE_DIR})
        message(STATUS "Created library target: ${_emby_lib_target} (from ${SOURCES_VAR})")

        # Create a generic alias <LIB_PREFIX> for compatibility with existing code
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

    # candidate pattern resolutions (try in order until one yields results)
    set(_candidates "")

    # if the pattern is already absolute use as-is
    string(REGEX MATCH "^/" _is_abs "${pattern}")
    if(_is_abs)
        list(APPEND _candidates "${pattern}")
    else()
        # non-absolute pattern: try different roots
        # 1) as relative to CMAKE_CURRENT_SOURCE_DIR
        list(APPEND _candidates "${CMAKE_CURRENT_SOURCE_DIR}/${pattern}")
        # 2) as relative to CMAKE_SOURCE_DIR (project root)
        if(DEFINED CMAKE_SOURCE_DIR)
            list(APPEND _candidates "${CMAKE_SOURCE_DIR}/${pattern}")
        endif()
        # 3) as relative to EMBY_FOLDER if defined
        if(DEFINED EMBY_FOLDER)
            list(APPEND _candidates "${EMBY_FOLDER}/${pattern}")
        endif()
        # 4) as provided (may be a wildcard pattern that works by itself)
        list(APPEND _candidates "${pattern}")
    endif()

    set(_matched_paths "")
    set(_used_candidate "")
    foreach(_cand ${_candidates})
        # file(GLOB_RECURSE) requires the pattern in quotes
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
        # Fallback: try matching the glob pattern directly against the list elements
        # Convert glob pattern to regex (simple): '*' -> '.*', '?' -> '.', escape other metacharacters
        set(_glob_pattern "${pattern}")
        # If pattern is relative, normalize as before for better regex info
        string(REGEX MATCH "^/" _is_abs_fallback "${_glob_pattern}")
        if(NOT _is_abs_fallback)
            set(_glob_pattern_rel "${CMAKE_CURRENT_SOURCE_DIR}/${_glob_pattern}")
        else()
            set(_glob_pattern_rel "${_glob_pattern}")
        endif()

        # Prepare regex: escape regex special chars except * ?
        set(_regex "${_glob_pattern_rel}")
        # Escape regex special chars except * ?
        string(REGEX REPLACE "([\\.\+\^\$\(\)\[\]\{\}\|])" "\\\\\\1" _regex "${_regex}")
        string(REPLACE "*" ".*" _regex "${_regex}")
        string(REPLACE "?" "." _regex "${_regex}")

        # Get elements of the passed list
        set(_list_items2 "${${list_var_name}}")
        if(NOT _list_items2)
            message(STATUS "EMBY_UTILS: the list ${list_var_name} is empty or undefined (fallback)")
            return()
        endif()

        set(_found_any_fallback FALSE)
        foreach(_it ${_list_items2})
            string(REPLACE "\\\\" "/" _it_norm_fallback "${_it}")
            string(REGEX REPLACE "/+$" "" _it_norm_fallback "${_it_norm_fallback}")
            # match with regex (case sensitive)
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

    # Get the elements of the passed list
    set(_list_items "${${list_var_name}}")
    if(NOT _list_items)
        message(STATUS "EMBY_UTILS: the list ${list_var_name} is empty or undefined")
        return()
    endif()

    set(_found_any FALSE)
    foreach(_p ${_matched_paths})
        # Normalize separators and remove trailing slashes
        string(REPLACE "\\\\" "/" _p_norm "${_p}")
        string(REGEX REPLACE "/+$" "" _p_norm "${_p_norm}")
        foreach(_it ${_list_items})
            string(REPLACE "\\\\" "/" _it_norm "${_it}")
            string(REGEX REPLACE "/+$" "" _it_norm "${_it_norm}")

            # comparisons: exact or substring
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
        # also print first entry of the list for debug
        list(GET _list_items 0 _first_item)
        message(STATUS "EMBY_UTILS: sample list item: ${_first_item}")
    endif()
endmacro()




macro(EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE folder2remove)
    # get include dirs and remove inclusion (robust against trailing slashes and separators)
    if("${folder2remove}" STREQUAL "")
        return()
    endif()
    # Normalize separators
    string(REPLACE "\\\\" "/" _f_norm "${folder2remove}")
    # Remove any trailing slash
    string(REGEX REPLACE "/+$" "" _f_no_slash "${_f_norm}")
    set(_variants "${_f_no_slash}" "${_f_no_slash}/")

    # Read includes of the current DIRECTORY
    get_property(dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
    if(NOT dirs)
        message(STATUS "EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE: no include directories set; nothing to remove")
        return()
    endif()

    set(_removed_any FALSE)

    # For each normalized variant, compare with current entries normalizing them as well
    foreach(_variant ${_variants})
        # normalize the variant for comparisons (remove backslashes and trailing slashes)
        string(REPLACE "\\\\" "/" _variant_norm "${_variant}")
        string(REGEX REPLACE "/+$" "" _variant_norm "${_variant_norm}")

        # check each existing entry
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

    # Remove any leftover duplicates and update the property
    list(REMOVE_DUPLICATES dirs)
    set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES ${dirs})

ENDMACRO()



# Alternative macro for local context
macro(EMBY_UTILS_GET_FOLDER_NAME path out_var)
    get_filename_component(_emby_folder_name "${path}" NAME)
    set(${out_var} "${_emby_folder_name}")
endmacro()
