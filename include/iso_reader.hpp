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
    bool close();                                              // The plugin must flush, write, cleanup... when this method is called
    bool isOK();                                               // Method to check if the plugin have any error
    bool getError(char *error, unsigned long long buffersize); // Get the last error string. Will be empty if there was no errors.
    void clearError();                                         // Clear the las error.

    // Reader
    bool openReader(char *filename, unsigned int threads = 1);            // Open the source/destination file
    bool seek(unsigned long long position, unsigned int mode);            // Seek method for the entire file
    bool seekCurrentDisk(unsigned long long position, unsigned int mode); // Seek method for the current disk
    unsigned long long tell();                                            // Tell method to get the current position of the whole file
    unsigned long long tellCurrentDisk();                                 // Tell method to get the current position of the current disk
    unsigned long long readData(char *output, unsigned long long toRead); // Read method. Must return the original disk data, after decompress, decode...
    unsigned int getTotalDisks();                                         // Return the total disks in this input/output file
    unsigned int getCurrentDisk();                                        // Return disk that is selected to be readed
    bool getID(char *id, unsigned long long buffersize);                  // Get the file Game ID
    bool getDiskID(char *id, unsigned long long buffersize);              // Get the current disk ID.
    bool changeCurrentDisk(unsigned int disk);                            // Change the disk that will be readed. Returns true if everything is OK and False if anything happens (for example, selected disk doesn't exists).

    // FileIO
    std::ifstream input_file;

protected:
    // Error management
    void setLastError(std::string error);
    void setLastError(char *error);
    char *last_error = NULL;
    bool is_ok = true;

    // ID
    char *gameID = NULL;
};

// C functions definition
extern "C"
{
    void SHARED_EXPORT *load();
    void SHARED_EXPORT unload(void *ptr);
    unsigned int SHARED_EXPORT getType();
    bool SHARED_EXPORT getPluginName(char *name, unsigned long long buffersize);
    bool SHARED_EXPORT getPluginVersion(char *version, unsigned long long buffersize);

    bool SHARED_EXPORT close(void *handler);                                                           // The plugin must flush, write, cleanup... when this method is called
    bool SHARED_EXPORT seek(void *handler, unsigned long long position, unsigned int mode);            // Seek method for the entire file
    bool SHARED_EXPORT seekCurrentDisk(void *handler, unsigned long long position, unsigned int mode); // Seek method for the current disk
    unsigned long long SHARED_EXPORT tell(void *handler);                                              // Tell method to get the current position of the whole file
    unsigned long long SHARED_EXPORT tellCurrentDisk(void *handler);                                   // Tell method to get the current position of the current disk
    bool SHARED_EXPORT getID(void *handler, char *id, unsigned long long buffersize);                  // Get the file Game ID
    bool SHARED_EXPORT getDiskID(void *handler, char *id, unsigned long long buffersize);              // Get the current disk ID.
    bool SHARED_EXPORT getTitle(void *handler, char *title, unsigned long long buffersize);            // Get the title of the game (ISO doesn't have it)
    bool SHARED_EXPORT getDiskTitle(void *handler, char *title, unsigned long long buffersize);        // Get the title of the disk (ISO doesn't have it)
    bool SHARED_EXPORT isOK(void *handler);                                                            // Method to check if the plugin have any error
    bool SHARED_EXPORT getError(void *handler, char *error, unsigned long long buffersize);            // Get the last error string. Will be empty if there was no errors.
    void SHARED_EXPORT clearError(void *handler);                                                      // Clear the las error.
    unsigned int SHARED_EXPORT getTotalDisks(void *handler);                                           // Return the total disks in this input/output file
    unsigned int SHARED_EXPORT getCurrentDisk(void *handler);                                          // Return disk that is selected to be readed
    bool SHARED_EXPORT changeCurrentDisk(void *handler, unsigned int disk);                            // Change the disk that will be readed. Returns true if everything is OK and False if anything happens (for example, selected disk doesn't exists).
    bool SHARED_EXPORT openReader(void *handler, char *filename, unsigned int threads = 1);            // Open the source file
    unsigned long long SHARED_EXPORT read(void *handler, char *output, unsigned long long toRead);     // Read method. Must return the original disk data, after decompress, decode...
    bool SHARED_EXPORT getCompatibleExtensions(char *extensions, unsigned long long buffersize);       // Return a vector of strings with the compatible extensions. Example: ["iso", "bin"]
}

#endif // _PLUGIN_HPP_H_