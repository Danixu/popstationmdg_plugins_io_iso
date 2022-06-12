@echo off

echo "Compiling the Windows version of the library"
cl.exe /LD /DBUILD_LIB /std:c++17 /EHsc /OUT:bin/windows/iso_reader_w.dll thirdparty\popstationmdg\src\plugins\export.cpp src\iso_reader.cpp /Iinclude /Ithirdparty\popstationmdg\src\plugins\