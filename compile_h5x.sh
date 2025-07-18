# Clean and build for STM32H5 development
./cleanall.sh

# Create build directory if it doesn't exist
mkdir -p build
cd build

cmake \
    -DEMBY_FOLDER=$PWD/../emby \
    -DEMBY_CONFIG=../src/emby_config.h \
    -DSTM32xx_Baremetal_DEVICE=STM32H563xx \
    -DSTM32xx_Baremetal_FAMILY=STM32H5xx \
    -DEMBY_PLATFORM=STM32xx_Baremetal \
    -DSTM32xx_Baremetal_STARTUP=${PWD}/../src/platform/arm/ST/nucleo_stm32H563ZI/startup_stm32h563zitx.s \
    -DSTM32xx_Baremetal_CONF_DIR=${PWD}/../src/platform/arm/ST/nucleo_stm32H563ZI/ \
    -DSTM32xx_Baremetal_LINKER=${PWD}/../src/platform/arm/ST/nucleo_stm32H563ZI/STM32H563ZITX_FLASH.ld \
    ..

# Check if cmake was successful
if [ $? -eq 0 ]; then
    echo "✅ CMake configuration completed successfully!"
    echo "You can now proceed with: cd build && make"
else
    echo "❌ CMake configuration failed!"
    exit 1
fi
