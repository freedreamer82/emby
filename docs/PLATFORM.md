# Platform guide for Emby

This guide explains how the "platforms" part of the Emby project is organized, the role of `Impl` folders, how to implement a platform CMake file (`emby-platform.cmake`) and the useful macros provided by the build framework.

Contents
- Architectural overview
- Conventions for `Impl` folders
- How to override core headers/implementations with the platform
- Common platform configuration variables (e.g. `_DEVICE`, `_FAMILY`, `_STARTUP`, `_LINKER`, `_CONF_DIR`)
- Typical structure of `emby-platform.cmake` (examples)
- Useful macros and helpers (with usage examples)
- Best practices and common pitfalls

Architectural overview
----------------------
The project is organized into two main areas:
- `emby/core`: contains the framework core code. Each core module (e.g. `EmbySystem`, `EmbyThreading`, `EmbyLibs`) provides default headers and sources.
- `emby/platforms/<PlatformName>`: contains platform-specific files (drivers, alternative implementations, startup files, linker scripts, etc.).

The key mechanism is that a platform can provide specific implementations for individual core components. To keep overrides organized the `Impl` convention is used.

Conventions for `Impl` folders
-----------------------------
Each core module can have an `Impl` subfolder with subcomponents that a platform may replace.
Example:

- `core/EmbySystem/Impl/Timer/Timer.hh`  (core default)
- `platforms/STM32xx_Baremetal/EmbySystem/Impl/Timer/Timer.hh`  (platform implementation)

Rules:
- If the platform provides `platform/.../EmbySystem/Impl/Timer/*.hh`, those headers are preferred over the corresponding `core/.../EmbySystem/Impl/Timer` headers.
- Helper CMake macros adjust include paths to achieve this substitution.

How the platform overrides core `Impl` files
-------------------------------------------
The file `emby/emby-core.cmake` contains two important macros:

- `EMBY_INCLUDE_ONLY_IMPL_DIRS(root_folder, debug_mode)`
  - Finds all `*/Impl/*/` directories under `root_folder` and adds those subdirectories to the include paths. This exposes the `Impl` headers from core.

- `EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(platform_root)`
  - Scans `platform_root/*/Impl/*/` and, if the platform provides `.hh` headers for a given module/subdir, removes the corresponding `core/<Module>/Impl/<Subdir>` directory from the include paths. This causes includes that previously resolved to `core/.../Impl/...` to resolve to the platform-provided headers instead.
  - Typical use: call `EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(${EMBY_STM32_BAREMETAL_PLATFORM})` within a platform CMake file to prefer platform headers.

Common platform configuration variables
--------------------------------------
The project uses several common variables when invoking CMake to select platform specifics:
- `EMBY_PLATFORM` — the platform name (e.g. `STM32xx_Baremetal`).
- `${EMBY_PLATFORM}_DEVICE` — the device macro (e.g. `STM32H563xx`) used for compile definitions and device-specific headers.
- `${EMBY_PLATFORM}_FAMILY` — family name (e.g. `STM32H5xx`, `STM32H7xx`) used to include the right CMSIS/driver files.
- `${EMBY_PLATFORM}_STARTUP` — the startup (assembly) file path to include in SOURCES.
- `${EMBY_PLATFORM}_LINKER` — the platform linker script (.ld).
- `${EMBY_PLATFORM}_CONF_DIR` — configuration directory (e.g. `Drivers/hal_conf`).

These variables are usually passed via build scripts (for example `compile_h5x.sh`) or as command-line arguments to CMake.

Typical structure of `emby-platform.cmake`
-----------------------------------------
A platform file (`emby/platforms/<P>/emby-platform.cmake`) typically does the following:
- Sets environment variables and compile-time definitions (e.g. `EMBY_BUILD_ARM`).
- Includes the platform `toolchain.cmake`.
- Validates required variables (`_FAMILY`, `_DEVICE`, `_STARTUP`, `_LINKER`, `_CONF_DIR`) and uses them to add SOURCES and include directories.
- Calls `EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT` to prefer platform implementations over core ones.
- Uses `EMBY_ADD_SOURCES` to gather platform and core sources.
- Creates the `emby_platform` library (or suffixed variant) for linking.

Minimal real-world example (extracted):

```cmake
set(EMBY_STM32_BAREMETAL_PLATFORM ${CMAKE_CURRENT_LIST_DIR})
add_compile_definitions(EMBY_BUILD_ARM)
include("${EMBY_FOLDER}/toolchain.cmake")

# Required checks
if (${EMBY_PLATFORM}_DEVICE)
    add_definitions(-DUSE_HAL_DRIVER -D${${EMBY_PLATFORM}_DEVICE})
else()
    message(FATAL_ERROR "include device i.e, ${EMBY_PLATFORM}_DEVICE=...")
endif()

# Include config dir and startup
list(APPEND SOURCES ${${EMBY_PLATFORM}_STARTUP})
include_directories(${${EMBY_PLATFORM}_CONF_DIR})
set(LINKER_SCRIPT ${${EMBY_PLATFORM}_LINKER})

# Prefer platform Impl over core Impl
EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(${EMBY_STM32_BAREMETAL_PLATFORM})
EMBY_INCLUDE_ONLY_IMPL_DIRS(${EMBY_STM32_BAREMETAL_PLATFORM} TRUE)

# Collect platform sources (drivers/startup/...)
file(GLOB_RECURSE EMBY_PLAT_SOURCES
    "${EMBY_STM32_BAREMETAL_PLATFORM}/Startup/*/*.c"
    "${EMBY_STM32_BAREMETAL_PLATFORM}/Drivers/.../*.c"
)
EMBY_ADD_SOURCES(${EMBY_STM32_BAREMETAL_PLATFORM} EMBY_PLAT_SOURCES)
list(APPEND EMBY_SOURCES ${EMBY_PLAT_SOURCES})

# Create platform library (uses core macro)
EMBY_CREATE_NAMED_LIB(emby_platform ${EMBY_STM32_BAREMETAL_PLATFORM} ${EMBY_STM32_BAREMETAL_PLATFORM} EMBY_PLAT_SOURCES)

# Append to project SOURCES
list(APPEND SOURCES ${EMBY_SOURCES})
```

Macros and helpers
------------------
The following macros live in `emby/emby-core.cmake` and in platform CMake files:

- `EMBY_INCLUDE_ONLY_IMPL_DIRS(root_folder, debug_mode)`
  - Adds only `*/Impl/*/` subdirectories under `root_folder` to the include paths.

- `EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(platform_root)`
  - Removes corresponding `core/.../Impl/...` subdirectories from include paths if the platform provides the same subpath (it detects presence using `.hh` files).

- `EMBY_ADD_SOURCES(START_ROOT OUT_VAR)`
  - Recursively searches `.cc`/`.c` under `START_ROOT` for predefined core modules (e.g. `EmbyConsole`, `EmbySystem`, etc.) and populates `OUT_VAR` with found sources. Useful to collect core or platform sources.

- `EMBY_ADD_TESTS(START_ROOT)`
  - Scans for test directories and creates executables `Test_<name>` for each subfolder that contains test source files.

- `EMBY_CREATE_NAMED_LIB(LIB_PREFIX SUFFIX_DIR INCLUDE_DIR SOURCES_VAR)`
  - Creates a static library named `<LIB_PREFIX>_<suffix_basename>` (for example `emby_core_Emby_X86`) from the sources in `SOURCES_VAR`. Also creates a generic alias `LIB_PREFIX` for backward compatibility if it doesn't already exist.

- `SUBDIRLIST` and `EMBY_INCLUDE_SUB_DIRS` — helpers to enumerate and include subdirectories.

Example: create the `emby_platform` library for the current platform
-------------------------------------------------------------------
After collecting platform sources (in `EMBY_PLAT_SOURCES` or `EMBY_SOURCES`) you can call:

```cmake
EMBY_CREATE_NAMED_LIB(emby_platform ${EMBY_STM32_BAREMETAL_PLATFORM} ${EMBY_STM32_BAREMETAL_PLATFORM} EMBY_PLAT_SOURCES)
```

This creates a static target `emby_platform_<suffix>` and an alias `emby_platform`. The top-level `CMakeLists.txt` links `emby_core` and `emby_platform` into the main executable when those targets exist.

Best practices and common pitfalls
---------------------------------
- Header/impl consistency: ensure that functions declared in `core/.../System.hh` have compatible implementations in the platform (same signature and namespace). Mismatched signatures or platform source compile errors often produce "undefined reference" at link time.
- When you provide implementations in `platform/.../<Module>/Impl/...`, call `EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT` so the platform headers are used instead of core ones.
- Prefer `target_include_directories` and `target_link_libraries` for new targets to keep include and link dependencies explicit. The project contains many global includes for historical reasons; try to encapsulate new targets where practical.
- Validate that variables such as `LINKER_SCRIPT`, `STARTUP`, and device definitions are actually set; platform CMake files often use `message(FATAL_ERROR ...)` to stop the configuration if something is missing.
- If you add startup assembly files or linker scripts, make sure they are added to `SOURCES` so they are considered by the build system.

Practical example (complete snippet for `emby/platforms/MyBoard/emby-platform.cmake`)
------------------------------------------------------------------------------------
```cmake
set(EMBY_MYBOARD_PLATFORM ${CMAKE_CURRENT_LIST_DIR})
include("${EMBY_FOLDER}/toolchain.cmake")
add_compile_definitions(EMBY_BUILD_MYBOARD)

# required checks
if(NOT DEFINED ${EMBY_PLATFORM}_DEVICE)
    message(FATAL_ERROR "Set ${EMBY_PLATFORM}_DEVICE")
endif()

# include conf/startup/linker
list(APPEND SOURCES ${${EMBY_PLATFORM}_STARTUP})
include_directories(${${EMBY_PLATFORM}_CONF_DIR})
set(LINKER_SCRIPT ${${EMBY_PLATFORM}_LINKER})

# prefer platform implementations
EMBY_CHANGE_IMPL_INCLUDE_FROM_EMBY_ROOT(${EMBY_MYBOARD_PLATFORM})
EMBY_INCLUDE_ONLY_IMPL_DIRS(${EMBY_MYBOARD_PLATFORM} TRUE)

# collect platform sources
file(GLOB_RECURSE EMBY_PLAT_SOURCES
    "${EMBY_MYBOARD_PLATFORM}/Drivers/**/*.c"
    "${EMBY_MYBOARD_PLATFORM}/Startup/**/*.s"
)
EMBY_ADD_SOURCES(${EMBY_MYBOARD_PLATFORM} EMBY_PLAT_SOURCES)

# create platform library
EMBY_CREATE_NAMED_LIB(emby_platform ${EMBY_MYBOARD_PLATFORM} ${EMBY_MYBOARD_PLATFORM} EMBY_PLAT_SOURCES)

# append to global sources
list(APPEND SOURCES ${EMBY_SOURCES})
```

Conclusion
----------
This guide should help you:
- Understand the `Impl` structure and how to override core modules with platform-specific implementations;
- Write an `emby-platform.cmake` that gathers platform sources, includes drivers/startup and creates the platform library;
- Use the core macros to manage include paths and gather sources consistently.

If you want, I can:
- Generate a commented `emby-platform.cmake` template ready to copy for a new platform.
- Add practical `Impl` examples (a couple of headers and sources) to demonstrate overriding core behavior.
