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
# Compile the library
echo -e "\tCompiling the Library"
g++ -g \
    -Iinclude \
    -Ithirdparty \
    -Ithirdparty/popstationmdg/include/ \
    -Ithirdparty/popstationmdg/thirdparty/ \
    -std=c++17 -ffunction-sections -fPIC -shared -static-libgcc -static-libstdc++ -g \
    src/iso_common.cpp \
    src/iso_reader.cpp \
    src/iso_writer.cpp \
    -o bin/linux/iso.so

echo -e "\tCompiling the Test Programs (Reader)"
g++ -g \
    -Iinclude \
    -Ithirdparty \
    -Ithirdparty/popstationmdg/include/ \
    -Ithirdparty/popstationmdg/thirdparty/ \
    thirdparty/popstationmdg/src/plugins/export.cpp \
    thirdparty/popstationmdg/src/plugins/plugin_handler.cpp \
    -ldl -pthread \
    src/test_reader.cpp \
    -static-libgcc -static-libstdc++ -std=c++17 -g \
    -o bin/linux/test_reader

echo -e "\tCompiling the Test Programs (Writer)"
g++ -g \
    -Iinclude \
    -Ithirdparty \
    -Ithirdparty/popstationmdg/include/ \
    -Ithirdparty/popstationmdg/thirdparty/ \
    thirdparty/popstationmdg/src/plugins/export.cpp \
    thirdparty/popstationmdg/src/plugins/plugin_handler.cpp \
    -ldl -pthread -static-libgcc -static-libstdc++ -std=c++17 -g \
    src/test_writer.cpp \
    -o bin/linux/test_writer

cp data/test.iso bin/linux/test.iso


###################
# Windows version #
###################

# Compile the library
echo "Compiling the Windows version"
# Compile the library
echo -e "\tCompiling the Library"
x86_64-w64-mingw32-g++-posix -g \
    -Iinclude \
    -Ithirdparty \
    -Ithirdparty/popstationmdg/include/ \
    -Ithirdparty/popstationmdg/thirdparty/ \
    -std=c++17 -fPIC -DBUILD_LIB -shared -static-libgcc -static-libstdc++ --static -O3 -s \
    thirdparty/popstationmdg/src/plugins/export.cpp \
    src/iso_common.cpp \
    src/iso_reader.cpp \
    src/iso_writer.cpp \
    -o bin/windows/iso.dll

echo -e "\tCompiling the Test Programs (Reader)"
# Main Program
x86_64-w64-mingw32-g++-posix -g \
    -Iinclude \
    -Ithirdparty \
    -Ithirdparty/popstationmdg/include/ \
    -Ithirdparty/popstationmdg/thirdparty/ \
    thirdparty/popstationmdg/src/plugins/export.cpp \
    thirdparty/popstationmdg/src/plugins/plugin_handler.cpp \
    src/test_reader.cpp \
    -static-libgcc -static-libstdc++ -std=c++17 -O3 -s \
    -o bin/windows/test_reader.exe

echo -e "\tCompiling the Test Programs (Writer)"
x86_64-w64-mingw32-g++-posix -g \
    -Iinclude \
    -Ithirdparty \
    -Ithirdparty/popstationmdg/include/ \
    -Ithirdparty/popstationmdg/thirdparty/ \
    thirdparty/popstationmdg/src/plugins/export.cpp \
    thirdparty/popstationmdg/src/plugins/plugin_handler.cpp \
    src/test_writer.cpp \
    -static-libgcc -static-libstdc++ -std=c++17 -O3 -s \
    -o bin/windows/test_writer.exe

cp data/test.iso bin/windows/test.iso
