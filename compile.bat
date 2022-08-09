@echo off

echo "Compiling the Windows version of the library"
cl.exe /LD /DBUILD_LIB /std:c++17 /EHsc /Fo:build/windows/ /Fe:bin/windows/iso.dll ^
    thirdparty\popstationmdg\src\plugins\export.cpp ^
    src\iso_reader.cpp src\iso_writer.cpp src\iso_common.cpp ^
    /Iinclude ^
    /Ithirdparty/popstationmdg/thirdparty ^
    /Ithirdparty/popstationmdg/src/plugins/ ^
    /Ithirdparty/popstationmdg/include/ ^
    /Ithirdparty/popstationmdg/include/plugins/