#!/bin/bash

## Common folders
mkdir -p build/windows
mkdir -p build/linux
mkdir -p bin/windows
mkdir -p bin/linux

#################
# Linux version #
#################

# Compile the library
echo "Compiling the Linux version"
#g++ -O3 -Iinclude -Ithirdparty/popstationmdg/src/plugins/ -std=c++17 -fPIC -g -static-libgcc -static-libstdc++ -c src/iso_reader.cpp -o build/linux/iso_reader.o
#g++ -O3 -std=c++17 -ffunction-sections -fdata-sections -shared -o bin/linux/iso_reader.so build/linux/iso_reader.o
#cp data/test.iso bin/linux/test.iso


###################
# Windows version #
###################

# Compile the library
echo "Compiling the Windows version"
x86_64-w64-mingw32-g++ -g -Iinclude -Ithirdparty/popstationmdg/src/plugins/ -std=c++17 -fPIC -DBUILD_LIB -shared -static-libgcc -static-libstdc++ thirdparty/popstationmdg/src/plugins/export.cpp src/iso_reader.cpp -o bin/windows/iso_reader.dll

echo "Compiling the test program"
# Main Program
x86_64-w64-mingw32-g++ -g -Ithirdparty/popstationmdg/src/plugins/ thirdparty/popstationmdg/src/plugins/export.cpp thirdparty/popstationmdg/src/logger/logger.cpp thirdparty/popstationmdg/src/plugins/plugin_handler.cpp src/test.cpp -static-libgcc -static-libstdc++ -std=c++17 -o bin/windows/test.exe
cp data/test.iso bin/windows/test.iso