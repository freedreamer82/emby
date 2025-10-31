# Emby

<p align="center">
  <img src="emby/misc/logo/emby.png" alt="Emby logo" width="240"/>
</p>

## What is Emby?

Emby is a small modular embedded framework written in C/C++ that provides a collection of core modules (logging, time, threading, console, system abstractions, small libraries) plus platform-specific implementations. The project is structured to keep a clear separation between platform-independent core code and platform-specific code (drivers, startup, linker scripts, HAL adapters).

Key features:
- Modular core components under `emby/core` (e.g. `EmbySystem`, `EmbyThreading`, `EmbyLibs`).
- Platform implementations under `emby/platforms/<PlatformName>`, optionally overriding `Impl` subcomponents from the core.
- CMake-based build system with helper macros to assemble platform and core sources.

## Repository layout (short)

- `emby/` — core sources and platform cmake helpers.
- `emby/platforms/` — platform-specific folders (e.g. `X86_Unix`, `STM32xx_Baremetal`, `Mbed`).
- `src/` — application code that consumes Emby core.
- `build/` — out-of-source build directory (we use per-platform subfolders here).

## How to build

This repository uses out-of-source builds. We recommend using one build directory per platform to avoid configuration conflicts.

### Build directories (recommended)
- `build/X86_Unix/` — for x86 development
- `build/STM32xx_Baremetal/STM32H5xx/` — for STM32 H5 family
- `build/STM32xx_Baremetal/STM32H7xx/` — for STM32 H7 family

### Building for x86 (Unix)

From the repository root you can configure a dedicated build directory:

```bash
# create and configure build dir for x86
./compile_x86.sh
# then build
cd build/X86_Unix && make -j$(nproc)
```

Or run cmake directly from the root using the recommended -S/-B syntax:

```bash
cmake -S . -B build/X86_Unix -DEMBY_FOLDER=$PWD/emby -DEMBY_CONFIG=$PWD/src/emby_config.h -DEMBY_PLATFORM=X86_Unix
cmake --build build/X86_Unix -- -j$(nproc)
```

### Building for ARM (STM32)

First, set up your ARM cross toolchain by exporting the path to your arm-none-eabi GCC toolchain:

```bash
export CUSTOM_GCC_TOOLCHAIN_PATH=/path/to/your/arm-none-eabi/bin
```

Then configure a build directory for the desired STM32 family. Example scripts are provided:

```bash
# H5 example
./compile_h5x.sh
cd build/STM32xx_Baremetal/STM32H5xx && make -j

# H7 example
./compile_h743.sh
cd build/STM32xx_Baremetal/STM32H7xx && make -j
```

Or invoke cmake directly:

```bash
cmake -S . -B build/STM32xx_Baremetal/STM32H5xx \
    -DEMBY_FOLDER=$PWD/emby \
    -DEMBY_CONFIG=$PWD/src/emby_config.h \
    -DSTM32xx_Baremetal_DEVICE=STM32H563xx \
    -DSTM32xx_Baremetal_FAMILY=STM32H5xx \
    -DEMBY_PLATFORM=STM32xx_Baremetal \
    -DSTM32xx_Baremetal_STARTUP=$PWD/src/platform/arm/ST/nucleo_stm32H563ZI/startup_stm32h563zitx.s \
    -DSTM32xx_Baremetal_CONF_DIR=$PWD/src/platform/arm/ST/nucleo_stm32H563ZI/ \
    -DSTM32xx_Baremetal_LINKER=$PWD/src/platform/arm/ST/nucleo_stm32H563ZI/STM32H563ZITX_FLASH.ld

cmake --build build/STM32xx_Baremetal/STM32H5xx -- -j
```

Notes:
- Use `-S <source> -B <build>` to avoid accidental configuration of the wrong directory.
- The `compile_*.sh` scripts in the repo set sensible defaults and create platform-specific build dirs.

## Platform implementation details

- Core modules live under `emby/core`. When a platform needs to provide a specialized implementation it typically creates the same module path inside `emby/platforms/<PlatformName>` and places platform headers or sources in an `Impl` subdirectory.
- The CMake helper macro `EMBY_UTILS_SYNC_IMPL_INCLUDES()` includes platform `Impl` subdirectories and removes matching `core/.../Impl/...` include directories when the platform provides the same subpath—this lets platform headers take precedence.
- Platform CMake files (usually named `emby-platform.cmake`) are responsible for:
  - adding platform-specific compile definitions and toolchain includes,
  - adding startup assembler and linker scripts,
  - collecting platform sources and drivers,
  - creating the `emby_platform` static library that is then linked with the core and application.

## Contributing

- If you add a new platform, create a `emby/platforms/<YourPlatform>/emby-platform.cmake` and follow the patterns used by `X86_Unix` and `STM32xx_Baremetal`.
- Provide `Impl` headers under the same module path if you want to override core components.
- Update the top-level scripts if you add new family/device combinations.

## Where to find help

Open an issue or create a PR describing the change and include reproduction steps. If you want assistance adding a new platform, point me to the board/family and I can create a starting platform template for you.

## Configurator tool

This repository includes a small helper script to quickly generate a build-time configuration header:

- `emby/emby/misc/tools/emby_config_menu.sh` — a simple text-based configurator using `dialog` that lets you set macros and values, then export them to `emby_config.h`.

See the detailed documentation for this tool in `docs/menu-tool.md` (usage, dependencies, extension points).

If the script is not executable, make it so and run it from the directory where you want `emby_config.h` to be created:

```bash
chmod +x emby/emby/misc/tools/emby_config_menu.sh
emby/emby/misc/tools/emby_config_menu.sh
```
