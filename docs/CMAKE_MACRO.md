# CMake macro documentation — emby-utils

This document lists and describes the helper macros defined in `emby/emby/emby-utils.cmake`.
For each macro you will find: signature, description, arguments and an example of usage.

---

## EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS(root_folder, debug_mode)
- Signature: `EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS root_folder debug_mode`
- Description: Searches for `*/Impl/*/` directories under `root_folder` and adds each found subdirectory to the global include paths (DIRECTORY property INCLUDE_DIRECTORIES). Useful to expose only the `Impl` folders of a given root (core or platform).
- Arguments:
  - `root_folder`: root folder to search from (e.g. `${EMBY_STM32_BAREMETAL_PLATFORM}`)
  - `debug_mode`: TRUE/FALSE; when TRUE prints included directories
- Example:
  - `EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS(${EMBY_STM32_BAREMETAL_PLATFORM} TRUE)`
- Notes: operates on the CMake DIRECTORY property INCLUDE_DIRECTORIES of the current directory.

---

## EMBY_UTILS_SYNC_IMPL_INCLUDES(platform_root [, core_root])
- Signature: `EMBY_UTILS_SYNC_IMPL_INCLUDES platform_root` (optional second argument `core_root` via ARGV1)
- Description: An all-in-one procedure to synchronize `Impl` implementations between platform and core:
  - includes the platform-provided `Impl` subdirectories (internally uses `EMBY_UTILS_INCLUDE_ONLY_IMPL_DIRS`)
  - for every platform `Impl` subdirectory that contains `.hh` headers, removes the corresponding `core/.../Impl/...` entry from global includes if it exists
  - optionally accepts a `core_root` (for example an absolute path to the core) to remove corresponding Impl directories in that core root first
- Arguments:
  - `platform_root`: path to the platform root
  - `core_root` (optional): alternative core root path to check before `EMBY_CORE_FOLDER`
- Example:
  - `EMBY_UTILS_SYNC_IMPL_INCLUDES(${EMBY_STM32_BAREMETAL_PLATFORM} "${STM32XX_BAREMETAL_DIR}")`
- Notes: if `core_root` is not provided the macro attempts to use the `EMBY_CORE_FOLDER` variable if defined; if none is available it will not remove core Impl entries but will still include the platform Impl directories.

---

## EMBY_UTILS_PRINT_GLOBAL_IMPL_INCLUDES()
- Signature: `EMBY_UTILS_PRINT_GLOBAL_IMPL_INCLUDES`
- Description: Scans the current directory's INCLUDE_DIRECTORIES property and builds the `IMPL_INCLUDE_DIRS` list containing only entries matching the pattern `.../Emby*/Impl...`. Also prints the directories found.
- Arguments: none
- Example: `EMBY_UTILS_PRINT_GLOBAL_IMPL_INCLUDES()`
- Notes: exposes the variable `IMPL_INCLUDE_DIRS` in the caller scope.

---

## EMBY_UTILS_INCLUDE_SUB_DIRS(root_folder, debug_mode)
- Signature: `EMBY_UTILS_INCLUDE_SUB_DIRS root_folder debug_mode`
- Description: Adds all subdirectories (multiple levels) under `root_folder` to the include paths. It relies on `EMBY_UTILS_SUBDIRLIST` to obtain the list of subdirectories.
- Arguments:
  - `root_folder`: root folder
  - `debug_mode`: TRUE/FALSE to enable debug output
- Example: `EMBY_UTILS_INCLUDE_SUB_DIRS(${EMBY_FOLDER}/core TRUE)`

---

## EMBY_UTILS_SUBDIRLIST(result curdir)
- Signature: `EMBY_UTILS_SUBDIRLIST result curdir`
- Description: Helper macro that expands up to 4 levels deep and sets `result` to a list of subdirectories (paths relative to `curdir`).
- Arguments:
  - `result`: name of the output variable
  - `curdir`: directory to scan
- Example:
  - `EMBY_UTILS_SUBDIRLIST(SUBDIRS "${EMBY_FOLDER}/core")`

---

## EMBY_UTILS_ADD_SOURCES(START_ROOT OUT_VAR)
- Signature: `EMBY_UTILS_ADD_SOURCES START_ROOT OUT_VAR`
- Description: Recursively searches for `.cc`/`.c` files in the core module folders (EmbyConsole, EmbyDebug, EmbyLibs, EmbyLog, EmbyMachine, EmbySystem, EmbyThreading, EmbyTime) starting from `START_ROOT` and populates the variable `OUT_VAR` with the found sources (duplicates removed). It gathers core or platform sources into a single list.
- Arguments:
  - `START_ROOT`: root directory for the search (typically `EMBY_FOLDER` or a platform root)
  - `OUT_VAR`: name of the variable that will be set with the list of sources
- Example:
  - `EMBY_UTILS_ADD_SOURCES(${EMBY_STM32_BAREMETAL_PLATFORM} EMBY_PLAT_SOURCES)`

---

## EMBY_UTILS_ADD_TESTS(START_ROOT)
- Signature: `EMBY_UTILS_ADD_TESTS START_ROOT`
- Description: Generates test executables for each subfolder under `START_ROOT/EmbyTest` (or for `START_ROOT` if it is already an `EmbyTest` folder). For each subfolder containing test sources it creates a `Test_<name>` target; it adds include directories and links `emby_core`/`emby_platform` if available.
- Arguments:
  - `START_ROOT`: test root folder or folder that contains `EmbyTest`
- Example: `EMBY_UTILS_ADD_TESTS(${EMBY_FOLDER})`
- Notes: sets the test RUNTIME_OUTPUT_DIRECTORY to `${CMAKE_CURRENT_BINARY_DIR}/tests`.

---

## EMBY_UTILS_CREATE_NAMED_LIB(LIB_PREFIX SUFFIX_DIR INCLUDE_DIR SOURCES_VAR)
- Signature: `EMBY_UTILS_CREATE_NAMED_LIB LIB_PREFIX SUFFIX_DIR INCLUDE_DIR SOURCES_VAR`
- Description: Creates a static library named `LIB_PREFIX_<suffix_basename>` using the sources contained in the variable `SOURCES_VAR`. Adds `INCLUDE_DIR` as include directories to the target and creates a generic alias `LIB_PREFIX` if it does not already exist.
- Arguments:
  - `LIB_PREFIX`: library prefix (e.g. `emby_platform`)
  - `SUFFIX_DIR`: directory used to produce the suffix (the basename is sanitized)
  - `INCLUDE_DIR`: directory to add as include to the target
  - `SOURCES_VAR`: name of the variable that contains the list of sources
- Example:
  - `EMBY_UTILS_CREATE_NAMED_LIB(emby_platform ${EMBY_MYBOARD_PLATFORM} ${EMBY_MYBOARD_PLATFORM} EMBY_PLAT_SOURCES)`
- Notes: if `SOURCES_VAR` is empty the macro skips target creation.

---

## EMBY_UTILS_REMOVE_FILES_FROM_LIST_WITH_WILDCARD(path listRemoveFrom)
- Signature: `EMBY_UTILS_REMOVE_FILES_FROM_LIST_WITH_WILDCARD path listRemoveFrom`
- Description: Performs a `file(GLOB path)` and removes each file found from the CMake list `listRemoveFrom` (useful to remove files from a `SOURCES` variable). Prints the removed files.
- Arguments:
  - `path`: wildcard pattern (e.g. `${CORE_DIR}/EmbyThreading/*.cc`)
  - `listRemoveFrom`: name of the list to remove items from
- Example:
  - `EMBY_UTILS_REMOVE_FILES_FROM_LIST_WITH_WILDCARD("${STM32XX_BAREMETAL_DIR}/EmbyThreading/*.cc" SOURCES)`

---

## EMBY_UTILS_PRINT_FILES_FROM_LIST_WITH_WILDCARD(pattern list_var_name)
- Signature: `EMBY_UTILS_PRINT_FILES_FROM_LIST_WITH_WILDCARD pattern list_var_name`
- Description: Checks whether the `pattern` (glob) resolves against the filesystem; if it does, compares the resolved results with the elements of the list `list_var_name` and prints the matches. If the glob does not resolve, attempts a fallback that converts the pattern to a regex and searches for matches inside the provided list.
- Arguments:
  - `pattern`: glob pattern or path (absolute or relative)
  - `list_var_name`: name of the CMake list to compare against
- Example:
  - `EMBY_UTILS_PRINT_FILES_FROM_LIST_WITH_WILDCARD("${SOME_PATTERN}" SOURCES)`

---

## EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE(folder2remove)
- Signature: `EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE folder2remove`
- Description: Removes a directory (or its variant with/without trailing slash) from the current directory's INCLUDE_DIRECTORIES property. Normalizes path separators before comparisons.
- Arguments:
  - `folder2remove`: path of the directory to remove
- Example:
  - `EMBY_UTILS_REMOVE_DIRS_FROM_INCLUDE(${EMBY_CORE_FOLDER}/EmbySystem/Impl/Timer)`

---

## EMBY_UTILS_GET_FOLDER_NAME(path out_var)
- Signature: `EMBY_UTILS_GET_FOLDER_NAME path out_var`
- Description: Simple wrapper around `get_filename_component(... NAME)` that sets `out_var` to the final folder name of `path`.
- Arguments:
  - `path`: input path
  - `out_var`: output variable name
- Example:
  - `EMBY_UTILS_GET_FOLDER_NAME(${EMBY_MYBOARD_PLATFORM} platform_name)`

---

If you want, I can:
- add longer examples (complete `emby-platform.cmake` snippets) for each macro;
- generate a compact version suitable to include directly into `emby/emby-core.cmake` as an inline reference comment.
