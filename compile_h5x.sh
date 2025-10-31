# Clean and build for STM32H5 development
./cleanall.sh

# Resolve repository root (script location)
REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"

# Verifica argomento obbligatorio: baremetal o freertos
if [ -z "$1" ]; then
    echo "Usage: $0 <baremetal|freertos>"
    exit 1
fi

case "$1" in
    baremetal)
        PLATFORM="STM32xx_Baremetal"
        ;;
    freertos)
        # Per FreeRTOS usiamo la piattaforma STM32H5xx_FreeRTOS
        PLATFORM="STM32xx_FreeRTOS"
        ;;
    *)
        echo "Invalid platform type: $1"
        echo "Usage: $0 <baremetal|freertos>"
        exit 1
        ;;
esac

FAMILY="STM32H5xx"
DEVICE="STM32H563xx"
BUILD_DIR="${REPO_ROOT}/build/${PLATFORM}/${FAMILY}"

# Create build directory if it doesn't exist
mkdir -p "${BUILD_DIR}"

cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
    -DEMBY_FOLDER=${REPO_ROOT}/emby \
    -DEMBY_CONFIG=${REPO_ROOT}/src/emby_config.h \
    -DSTM32xx_Baremetal_DEVICE=${DEVICE} \
    -DSTM32xx_Baremetal_FAMILY=${FAMILY} \
    -DEMBY_PLATFORM=${PLATFORM} \
    -DSTM32xx_Baremetal_STARTUP=${REPO_ROOT}/src/platform/arm/ST/nucleo_stm32H563ZI/startup_stm32h563zitx.s \
    -DSTM32xx_Baremetal_CONF_DIR=${REPO_ROOT}/src/platform/arm/ST/nucleo_stm32H563ZI/ \
    -DSTM32xx_Baremetal_LINKER=${REPO_ROOT}/src/platform/arm/ST/nucleo_stm32H563ZI/STM32H563ZITX_FLASH.ld

# Check if cmake was successful
if [ $? -eq 0 ]; then
    echo "✅ CMake configuration completed successfully in ${BUILD_DIR}!"
    echo "You can now proceed with: cd ${BUILD_DIR} && make"
else
    echo "❌ CMake configuration failed!"
    exit 1
fi
