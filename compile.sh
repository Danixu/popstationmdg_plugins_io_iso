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
g++ -O3 -Iinclude -std=c++17 -fPIC -g -static-libgcc -static-libstdc++ -c src/reader/iso_reader.cpp -o build/linux/iso_reader.o
g++ -O3 -std=c++17 -ffunction-sections -fdata-sections -shared -o bin/linux/iso_reader.so build/linux/iso_reader.o


###################
# Windows version #
###################

# Compile the library
x86_64-w64-mingw32-g++ -O3 -Iinclude -std=c++17 -fPIC -DBUILD_LIB -static-libgcc -static-libstdc++ -c src/reader/iso_reader.cpp -o build/windows/iso_reader.obj
x86_64-w64-mingw32-g++ -O3 -ffunction-sections -fdata-sections -DBUILD_LIB -shared -static-libgcc -static-libstdc++ -o bin/windows/iso_reader.dll build/windows/iso_reader.obj