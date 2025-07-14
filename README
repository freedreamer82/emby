# Emby Build Instructions

## Building for x86

```bash
cmake -DEMBY_FOLDER=$PWD/emby -DEMBY_CONFIG=src/emby_config.h -DEMBY_PLATFORM=X86_Unix .
```

## Building for ARM

### Setup ARM Toolchain

First, export the path to your ARM GCC toolchain:

```bash
export CUSTOM_GCC_TOOLCHAIN_PATH=<your ARM gcc folder>
```

Example:
```bash
export CUSTOM_GCC_TOOLCHAIN_PATH=/mnt/DATA/gcc-arm-none-eabi-10-2020-q4-major/bin/
```

### Build Command

```bash
cmake -DEMBY_FOLDER=$PWD/emby \
      -DEMBY_CONFIG=src/emby_config.h \
      -DSTM32L4x_FreeRTOS_DEVICE=STM32L431xx \
      -DEMBY_PLATFORM=STM32L4x_FreeRTOS \
      -DSTM32L4x_FreeRTOS_STARTUP=${PWD}/src/stm32l431cc/startup_stm32l431cctx.S \
      -DSTM32L4x_FreeRTOS_CONF_DIR=${PWD}/src/stm32l431cc/ \
      -DSTM32L4x_FreeRTOS_LINKER=${PWD}/src/stm32l431cc/STM32L431CCTX_FLASH.ld \
      .
```
