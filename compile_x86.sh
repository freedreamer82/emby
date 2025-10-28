# Clean and build for x86 development
./cleanall.sh

# Resolve repository root
REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
PLATFORM="X86_Unix"
BUILD_DIR="${REPO_ROOT}/build/${PLATFORM}"

# Create build directory if it doesn't exist
mkdir -p "${BUILD_DIR}"

cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
    -DEMBY_FOLDER=${BUILD_DIR}/../emby \
    -DEMBY_CONFIG=${REPO_ROOT}/src/emby_config.h \
    -DEMBY_PLATFORM=${PLATFORM}

# Check if cmake was successful
if [ $? -eq 0 ]; then
    echo "✅ CMake configuration completed successfully in ${BUILD_DIR}!"
    echo "You can now proceed with: cd ${BUILD_DIR} && make"
else
    echo "❌ CMake configuration failed!"
    exit 1
fi