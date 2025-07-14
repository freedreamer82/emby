# Clean CMake artifacts from root directory
sudo rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makefile *.bin *.map *.hex *.elf emby-*

# Clean build directory used by compilation scripts
sudo rm -rf build/

# Clean cmake-build-debug directory (IDE generated)
sudo rm -rf cmake-build-debug/

