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
g++ -O3 -Iinclude -Ithirdparty/popstationmdg/src/plugins/ -Ithirdparty/popstationmdg/src/logger/ -std=c++17 -ffunction-sections -fPIC -g -static-libgcc -static-libstdc++ -shared thirdparty/popstationmdg/src/logger/logger.cpp src/iso_common.cpp src/iso_reader.cpp src/iso_writer.cpp -o bin/linux/iso.so
g++ -g -Ithirdparty/popstationmdg/src/plugins/ thirdparty/popstationmdg/src/plugins/export.cpp thirdparty/popstationmdg/src/logger/logger.cpp thirdparty/popstationmdg/src/plugins/plugin_handler.cpp -ldl -pthread src/test_reader.cpp -static-libgcc -static-libstdc++ -std=c++17 -o bin/linux/test_reader
g++ -g -Ithirdparty/popstationmdg/src/plugins/ thirdparty/popstationmdg/src/plugins/export.cpp thirdparty/popstationmdg/src/logger/logger.cpp thirdparty/popstationmdg/src/plugins/plugin_handler.cpp -ldl -pthread src/test_writer.cpp -static-libgcc -static-libstdc++ -std=c++17 -o bin/linux/test_writer
cp data/test.iso bin/linux/test.iso


###################
# Windows version #
###################

# Compile the library
echo "Compiling the Windows version"
x86_64-w64-mingw32-g++ -g -Iinclude -Ithirdparty/popstationmdg/src/plugins/ -Ithirdparty/popstationmdg/src/logger/ -std=c++17 -fPIC -DBUILD_LIB -shared -static-libgcc -static-libstdc++ thirdparty/popstationmdg/src/plugins/export.cpp thirdparty/popstationmdg/src/logger/logger.cpp src/iso_common.cpp src/iso_reader.cpp src/iso_writer.cpp -o bin/windows/iso.dll

echo "Compiling the test program"
# Main Program
x86_64-w64-mingw32-g++ -g -Ithirdparty/popstationmdg/src/plugins/ thirdparty/popstationmdg/src/plugins/export.cpp thirdparty/popstationmdg/src/logger/logger.cpp thirdparty/popstationmdg/src/plugins/plugin_handler.cpp src/test_reader.cpp -static-libgcc -static-libstdc++ -std=c++17 -o bin/windows/test_reader.exe
x86_64-w64-mingw32-g++ -g -Ithirdparty/popstationmdg/src/plugins/ thirdparty/popstationmdg/src/plugins/export.cpp thirdparty/popstationmdg/src/logger/logger.cpp thirdparty/popstationmdg/src/plugins/plugin_handler.cpp src/test_writer.cpp -static-libgcc -static-libstdc++ -std=c++17 -o bin/windows/test_writer.exe
cp data/test.iso bin/windows/test.iso