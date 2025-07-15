# Clean and build for x86 development
./cleanall.sh

# Create build directory if it doesn't exist
mkdir -p build
cd build

cmake \
    -DEMBY_FOLDER=${PWD}/../emby \
    -DEMBY_CONFIG=../src/emby_config.h \
    -DEMBY_PLATFORM=X86_Unix \
    ..

# Check if cmake was successful
if [ $? -eq 0 ]; then
    echo "✅ CMake configuration completed successfully!"
    echo "You can now proceed with: cd build && make"
else
    echo "❌ CMake configuration failed!"
    exit 1
fi