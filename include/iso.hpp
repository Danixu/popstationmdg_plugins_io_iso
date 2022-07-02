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
#include "plugin_assistant.hpp"

#ifndef _PLUGIN_HPP_H_
#define _PLUGIN_HPP_H_

class IsoReader
{
public:
    // Constructor and destructor
    IsoReader();
    ~IsoReader();

    // Common
    bool open(char *filename, bool writer = false, unsigned int threads = 1);
    bool close();
    bool isOK();
    bool getError(char *error, unsigned long long buffersize);
    void clearError();

    // Reader & Writer
    bool seek(unsigned long long position, unsigned int mode);
    bool seekCurrentDisk(unsigned long long position, unsigned int mode);
    unsigned long long tell();
    unsigned long long tellCurrentDisk();

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
    char *last_error = NULL;
    bool isOk = true;
    bool isWriter = false;

    // ID
    char *gameID = NULL;

    // FileIO
    std::ifstream input_file;
    std::ofstream output_file;
};

// C functions definition
extern "C"
{
    // Common
    void SHARED_EXPORT *load();
    void SHARED_EXPORT unload(void *ptr);
    unsigned int SHARED_EXPORT getType();
    bool SHARED_EXPORT getPluginName(char *name, unsigned long long buffersize);
    bool SHARED_EXPORT getPluginVersion(char *version, unsigned long long buffersize);
    bool SHARED_EXPORT open(void *handler, char *filename, bool writer = false, unsigned int threads = 1);
    bool SHARED_EXPORT close(void *handler);
    bool SHARED_EXPORT isOK(void *handler);
    bool SHARED_EXPORT getError(void *handler, char *error, unsigned long long buffersize);
    void SHARED_EXPORT clearError(void *handler);
    bool SHARED_EXPORT getCompatibleExtensions(char *extensions, unsigned long long buffersize);
    unsigned int SHARED_EXPORT getCurrentDisk(void *handler);
    unsigned int SHARED_EXPORT getTotalDisks(void *handler);
    bool SHARED_EXPORT seek(void *handler, unsigned long long position, unsigned int mode);
    bool SHARED_EXPORT seekCurrentDisk(void *handler, unsigned long long position, unsigned int mode);
    unsigned long long SHARED_EXPORT tell(void *handler);
    unsigned long long SHARED_EXPORT tellCurrentDisk(void *handler);

    // Reader
    unsigned long long SHARED_EXPORT readData(void *handler, char *output, unsigned long long toRead);
    bool SHARED_EXPORT getGameID(void *handler, char *id, unsigned long long buffersize);
    bool SHARED_EXPORT getDiskID(void *handler, char *id, unsigned long long buffersize);
    bool SHARED_EXPORT getGameTitle(void *handler, char *title, unsigned long long buffersize);
    bool SHARED_EXPORT getDiskTitle(void *handler, char *title, unsigned long long buffersize);
    bool SHARED_EXPORT changeCurrentDisk(void *handler, unsigned int disk);

    // Writer
    bool SHARED_EXPORT hasMultiDiskSupport();
    unsigned int SHARED_EXPORT getMaxDisks();
    bool SHARED_EXPORT setTotalDisks(void *handler, unsigned int totalDisks);
    unsigned long long SHARED_EXPORT writeData(void *handler, char *input, unsigned long long inputSize);
    bool SHARED_EXPORT setGameID(void *handler, char *gameID);
    bool SHARED_EXPORT setDiskID(void *handler, char *gameID);
    bool SHARED_EXPORT addNewDisk(void *handler);
    bool SHARED_EXPORT closeCurrentDisk(void *handler);
}

#endif // _PLUGIN_HPP_H_