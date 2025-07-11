# Create build directory if it doesn't exist
mkdir -p build
cd build

cmake \
    -DEMBY_FOLDER=$PWD/../emby \
    -DEMBY_CONFIG=../src/emby_config.h \
    -DSTM32L4x_FreeRTOS_DEVICE=STM32L431xx \
    -DEMBY_PLATFORM=STM32L4x_FreeRTOS \
    -DSTM32L4x_FreeRTOS_STARTUP=${PWD}/../src/stm32l431cc/startup_stm32l431cctx.S \
    -DSTM32L4x_FreeRTOS_CONF_DIR=${PWD}/../src/stm32l431cc/ \
    -DSTM32L4x_FreeRTOS_LINKER=${PWD}/../src/stm32l431cc/STM32L431CCTX_FLASH.ld \
    ..

echo "Complete! Now you can run make from the build directory"