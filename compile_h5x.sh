# Create build directory if it doesn't exist
mkdir -p build
cd build

cmake \
    -DEMBY_FOLDER=$PWD/../emby \
    -DEMBY_CONFIG=../src/emby_config.h \
    -DSTM32H5x_FreeRTOS_DEVICE=STM32H563xx \
    -DEMBY_PLATFORM=STM32H5x_FreeRTOS \
    -DSTM32H5x_FreeRTOS_STARTUP=${PWD}/../src/stmH563ZI/startup_stm32h563zitx.s \
    -DSTM32H5x_FreeRTOS_CONF_DIR=${PWD}/../src/stmH563ZI/ \
    -DSTM32H5x_FreeRTOS_LINKER=${PWD}/../src/stmH563ZI/STM32H563ZITX_FLASH.ld \
    ..

echo "Complete! Now you can run make from the build directory"
