/*

  This header file is the virtual plugin definition which will be used in derivated plugins and main program

*/

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <cstring>

#include "export.h"
#include "plugin_assistant.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifndef _PLUGIN_HPP_H_
#define _PLUGIN_HPP_H_

namespace PopstationmdgPlugin
{
    class IsoReader
    {
    public:
        // Constructor and destructor
        IsoReader();
        ~IsoReader();

        // Common
        bool open(char *filename, unsigned int mode = PTReader, unsigned int compressionLevel = 9, unsigned int threads = 1);
        bool close();
        bool isOK();
        bool getError(char *error, unsigned long long buffersize);
        void clearError();

        // Reader & Writer
        unsigned long long getDiskSize();
        unsigned long long getDiskRealSize();
        bool seek(unsigned long long position, unsigned int mode);
        bool seekCurrentDisk(unsigned long long position, unsigned int mode);
        unsigned long long tell();
        unsigned long long tellCurrentDisk();
        bool setSettings(char *settingsData, unsigned long &settingsSize, unsigned int mode = PTReader);

        // Reader
        unsigned long long readData(char *output, unsigned long long toRead);
        unsigned int getTotalDisks();
        unsigned int getCurrentDisk();
        bool getID(char *id, unsigned long long buffersize);
        bool getDiskID(char *id, unsigned long long buffersize);
        bool changeCurrentDisk(unsigned int disk);

        // Writer
        unsigned long long writeData(char *output, unsigned long long toWrite);
        bool addNewDisk();
        bool closeCurrentDisk();

    protected:
        // Error management
        void setLastError(std::string error);
        void setLastError(char *error);
        void freeReaderResources();
        void freeWriterResources();
        std::string getDiskFilename(uint8_t diskNumber);
        char *last_error = nullptr;
        bool isOk = true;
        PluginType pluginMode = PTNone;

        // ID
        char *gameID = nullptr;

        // Disk Size
        // Size in rest (compressed, optimized...)
        size_t diskSize = 0;
        // Final RAW data size
        size_t diskRealSize = 0;

        // FileIO
        std::ifstream input_file;
        std::ofstream output_file;

        // Cache settings
        bool cacheEnabled = false;
        unsigned long cacheSize = 235200; // 200 sectors
    };
}

#endif // _PLUGIN_HPP_H_