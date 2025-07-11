./cleanall.sh

# Create build directory if it doesn't exist
mkdir -p build
cd build


cmake \
    -DEMBY_FOLDER=$PWD/../emby \
    -DEMBY_CONFIG=../src/emby_config.h \
    -DSTM32L4x_FreeRTOS_DEVICE=STM32L431xx \
    -DEMBY_PLATFORM=STM32L4x_FreeRTOS \
    -DEMBY_APP_MACHINE_PATH=src/stm32l431cc/EmbyAppMachine.cc \
    -DSTM32L4x_FreeRTOS_STARTUP=${PWD}/../src/stm32l431cc/startup_stm32l431cctx.S \
    -DSTM32L4x_FreeRTOS_CONF_DIR=${PWD}/../src/stm32l431cc/ \
    -DSTM32L4x_FreeRTOS_LINKER=${PWD}/../src/stm32l431cc/STM32L431CCTX_FLASH.ld \
    ..

# Check if cmake was successful
if [ $? -eq 0 ]; then
    echo "✅ CMake configuration completed successfully!"
    echo "You can now proceed with: cd build && make"
else
    echo "❌ CMake configuration failed!"
    exit 1
fi
