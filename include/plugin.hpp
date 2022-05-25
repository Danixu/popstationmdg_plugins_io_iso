#pragma once

/*

  This header file is the virtual plugin definition which will be used in derivated plugins and main program

*/

#include "export.h"
#include <string>
#include <vector>
#include <memory>

enum PluginType
{
    PTReader = 1,
    PTWritter,
    PTFilter
};

enum PluginSeekMode
{
    PluginSeekMode_Begin = 1,
    PluginSeekMode_End,
    PluginSeekMode_Forward,
    PluginSeekMode_Backward
};

class SHARED_EXPORT Plugin
{
public:
    // Constructor and destructor
    virtual ~Plugin() = default;

    // Reader constructor example
    // Plugin(std::string inFilename);

    // Writter constructor example
    // Plugin(std::string outFilename);

    // Common
    virtual bool open(std::string filename, uint8_t threads = 1) = 0;       // Open the source/destination file
    virtual bool close() = 0;                                               // The plugin must flush, write, cleanup... when this method is called
    virtual bool seek(size_t position, PluginSeekMode mode) = 0;            // Seek method for the entire file
    virtual bool seekCurrentDisk(size_t position, PluginSeekMode mode) = 0; // Seek method for the current disk
    virtual size_t tell() = 0;                                              // Tell method to get the current position of the whole file
    virtual size_t tellCurrentDisk() = 0;                                   // Tell method to get the current position of the current disk
    virtual std::string getID() = 0;                                        // Get the file Game ID
    virtual std::string getDiskID() = 0;                                    // Get the current disk ID.
    virtual bool isOK() = 0;                                                // Method to check if the plugin have any error
    virtual std::string getError() = 0;                                     // Get the last error string. Will be empty if there was no errors.
    virtual void clearError() = 0;                                          // Clear the las error.
    virtual uint8_t getTotalDisks() = 0;                                    // Return the total disks in this input/output file
    virtual uint8_t getCurrentDisk() = 0;                                   // Return disk that is selected to be readed
    virtual bool changeCurrentDisk(uint8_t disk) = 0;                       // Change the disk that will be readed. Returns true if everything is OK and False if anything happens (for example, selected disk doesn't exists).

    // Reader
    virtual size_t read(uint8_t *output, size_t toRead) = 0;        // Read method. Must return the original disk data, after decompress, decode...
    virtual std::vector<std::string> getCompatibleExtensions() = 0; // Return a vector of strings with the compatible extensions. Example: ["iso", "bin"]

    // Writter
    virtual bool setID(std::string gameID) = 0;                    // Set the game ID required by some plugins.
    virtual bool write(uint8_t &input, size_t toWrite) = 0;        // Write method. This method will receive the raw disk data and will have to process it to the output format.
    virtual bool addDisk(size_t diskSize, std::string diskID) = 0; // Will be used on every disk insertion. diskSize will contains the source disk size (used for some calculations in PBP for example).
    virtual bool closeDisk() = 0;                                  // This method will close the current disk, writting all the required data

protected:
    std::string last_error = "";
    bool is_ok = true;
};

#define DEFINE_PLUGIN(classType, PType, pluginName, pluginVersion) \
    extern "C"                                                     \
    {                                                              \
        SHARED_EXPORT classType *load()                            \
        {                                                          \
            printf("Creating new class pointer\n");                \
            return new classType();                                \
        }                                                          \
                                                                   \
        SHARED_EXPORT void unload(classType *ptr)                  \
        {                                                          \
            delete ptr;                                            \
        }                                                          \
                                                                   \
        SHARED_EXPORT PluginType get_type()                        \
        {                                                          \
            return PType;                                          \
        }                                                          \
                                                                   \
        const char SHARED_EXPORT *name()                           \
        {                                                          \
            return pluginName;                                     \
        }                                                          \
                                                                   \
        const char SHARED_EXPORT *version()                        \
        {                                                          \
            return pluginVersion;                                  \
        }                                                          \
    }
