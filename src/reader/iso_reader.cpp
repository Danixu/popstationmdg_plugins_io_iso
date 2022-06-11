#include "iso_reader.hpp"

// Reader constructor
IsoReader::IsoReader()
{
    // Set the stream to throw an exception if failbit was set
    std::ios_base::iostate exceptionMask = input_file.exceptions() | std::ifstream::failbit | std::ifstream::badbit;
    input_file.exceptions(exceptionMask);

    // Fake GameID for testing purposes
    /*
    gameID = new char[11];
    strncpy_s(gameID, 11, "SCES00009", 11);
    */

    /*
        // Buffer Options
        PluginOption enableBuffer;
        options_order.push_back("enable_buffer");
        enableBuffer.name = "Enable read buffer";
        enableBuffer.value = false;
        options_items["enable_buffer"] = enableBuffer;

        PluginOption bufferSize;
        options_order.push_back("read_buffer_size");
        bufferSize.name = "Read buffer size";
        bufferSize.value = (uint64_t)131072; // 128k
        options_items["read_buffer_size"] = bufferSize;
        */
}

// Reader destructor
IsoReader::~IsoReader()
{
    if (gameID != NULL)
    {
        delete[] gameID;
        gameID = NULL;
    }

    if (last_error != NULL)
    {
        delete[] last_error;
        last_error = NULL;
    }

    /*
    options_order.clear();
    options_items.clear();
    */
}

// Open the ISO file
bool IsoReader::open(char *filename, unsigned int threads)
{
    // This reader is very simple and non CPU intensive, so threads are not required and will be ignored

    // Open source file
    try
    {
        input_file.open(filename, std::ifstream::binary);
        return true;
    }
    catch (std::ios_base::failure &e)
    {
        setLastError(std::string("There was an error opening the file: ") + std::string(e.what()));
        return false;
    }
}

// Close the ISO file (if was opened)
bool IsoReader::close()
{
    // Delete the ID which is not usefull anymore
    if (gameID != NULL)
    {
        delete[] gameID;
        gameID = NULL;
    }

    // Try to close the file
    try
    {
        input_file.close();
        return true;
    }
    catch (std::ios_base::failure &e)
    {
        setLastError(std::string("There was an error closing the file: ") + std::string(e.what()));
        return false;
    }
}

// Seek into the file
bool IsoReader::seek(unsigned long long position, unsigned int mode)
{
    auto seek_mode = std::ios::beg;

    if (mode == PluginSeekMode_End)
    {
        seek_mode = std::ios::end;
    }
    else if (mode == PluginSeekMode_Forward)
    {
        position += input_file.tellg();
    }
    else if (mode == PluginSeekMode_Backward)
    {
        position = uint64_t(input_file.tellg()) - position;
    }

    if (!input_file.seekg(position, seek_mode))
    {
        setLastError(std::string("There was an error seeking into the file"));
        return false;
    }
    return true;
}

// Same as above because ISO is just single disk format
bool IsoReader::seekCurrentDisk(unsigned long long position, unsigned int mode)
{
    return seek(position, mode);
}

// Get the current image position
unsigned long long IsoReader::tell()
{
    if (input_file.is_open())
    {
        return input_file.tellg();
    }
    else
    {
        setLastError(std::string("There is no file opened"));
        return 0;
    }
}

// Get the current image position
unsigned long long IsoReader::tellCurrentDisk()
{
    return tell();
}

// Get the disk ID (currently a dummy)
char *IsoReader::getID()
{
    if (gameID != NULL)
    {
        return gameID;
    }
    else
    {
        // No input file
        if (!input_file.is_open())
        {
            setLastError(std::string("There is no file opened"));
            return NULL;
        }

        // Get current position
        unsigned long long current_pos = tell();
        // There was an error
        if (!isOK())
        {
            setLastError(std::string("There was an error getting the current file position."));
            return NULL;
        }

        // Go to the start of the file
        if (!seek(0, PluginSeekMode_Begin))
        {
            setLastError(std::string("There was an error seeking to the start of the file."));
            return NULL;
        }

        // Reserve 200k of RAM to store the disk data
        char *disk_data = (char *)malloc(204800);

        if (disk_data == NULL)
        {
            setLastError(std::string("There was an error allocating the required memory."));
            return NULL;
        }

        // Read the first 200k of data into the new buffer
        size_t readed = readData(disk_data, 204800);
        if (!isOK())
        {
            std::free((void *)disk_data);
            return NULL;
        }

        // return to the last position
        if (!seek(current_pos, PluginSeekMode_Begin))
        {
            setLastError(std::string("There was an error seeking to the start of the file."));
            std::free((void *)disk_data);
            return NULL;
        }

        // Try to extract the game ID from those 200k
        for (size_t i = 0; i < readed; i++)
        {
            if (
                disk_data[i] == 0x53 &&
                (disk_data[i + 1] == 0x43 || disk_data[i + 1] == 'L') &&
                (disk_data[i + 2] == 0x45 || disk_data[i + 1] == 'U') &&
                disk_data[i + 3] == 0x53)
            {
                // Looks like we found it
                gameID = new char[10];
                std::memset(gameID, 0, 10);

                // Set the gameID. Normally in disk is XXXX_XX.XXX, so we will get only the code
                gameID[0] = disk_data[i];
                gameID[1] = disk_data[i + 1];
                gameID[2] = disk_data[i + 2];
                gameID[3] = disk_data[i + 3];
                gameID[4] = disk_data[i + 5];
                gameID[5] = disk_data[i + 6];
                gameID[6] = disk_data[i + 7];
                gameID[7] = disk_data[i + 9];
                gameID[8] = disk_data[i + 10];

                // Free the buffer before return the ID
                std::free((void *)disk_data);

                return gameID;
            }
        }

        // Nothing was found, so we will free the buffer and return NULL
        std::free((void *)disk_data);
        setLastError(std::string("No ID found."));

        return NULL;
    }
}

// In ISO files the ID and DiskID are the same (is just a disk)
char *IsoReader::getDiskID()
{
    return getID();
}

// Check if Status is OK
bool IsoReader::isOK()
{
    return is_ok;
}

// Get the last error
char *IsoReader::getError()
{
    return last_error;
}

// Clear the last error and isOK status
void IsoReader::clearError()
{
    delete[] last_error;
    last_error = NULL;
    is_ok = true;
}

// Get total disk will be always 1 if a disk is oppened
unsigned int IsoReader::getTotalDisks()
{
    if (input_file.is_open())
    {
        return 1;
    }
    else
    {
        setLastError(std::string("There is no file opened"));
        return 0;
    }
}

// Same as above
unsigned int IsoReader::getCurrentDisk()
{
    if (input_file.is_open())
    {
        return 1;
    }
    else
    {
        setLastError(std::string("There is no file opened"));
        return 0;
    }
}

// ChangeCurrentDisk is not available for this format.
bool IsoReader::changeCurrentDisk(unsigned int disk)
{
    return false;
}

// Read the input file data into the provided buffer. Return the readed bytes.
unsigned long long IsoReader::readData(char *output, unsigned long long toRead)
{
    if (!input_file.is_open())
    {
        // There is no open file
        setLastError(std::string("There is no file opened"));
        return 0;
    }

    // Try to read from file
    try
    {
        input_file.read(output, toRead);
        return input_file.gcount();
    }
    catch (std::ios_base::failure &e)
    {
        setLastError(std::string("There was an error reading from the file: ") + std::string(e.what()));
        return 0;
    }
}

// Return the compatible extensions.
const char *IsoReader::getCompatibleExtensions()
{
    return extensions;
}

void IsoReader::setLastError(std::string error)
{
    if (error.length() > 0)
    {
        char *error_char = new char[error.length() + 1];
        std::memset(error_char, 0, error.length() + 1);
        strncpy_s(error_char, error.length() + 1, error.c_str(), error.length());
        setLastError(error_char);
    }
}

// Set the last error text and isOK to false
void IsoReader::setLastError(char *error)
{
    if (last_error != NULL)
    {
        delete[] last_error;
        last_error = NULL;
    }

    last_error = error;
    is_ok = false;
}

extern "C"
{
    //
    // Creates a new plugin object in memory and return its address
    //
    void SHARED_EXPORT *load()
    {
        return (void *)new IsoReader();
    }

    //
    // Delete the plugin object. Must be called before unload the library or memory leaks will occur
    //
    void SHARED_EXPORT unload(void *ptr)
    {
        delete (IsoReader *)ptr;
    }

    //
    // Get the type of plugin to allow to filter
    //
    unsigned int SHARED_EXPORT getType()
    {
        return PTReader;
    }

    //
    // Return the plugin name
    //
    const char SHARED_EXPORT *getPluginName()
    {
        return "ISO Image";
    }

    //
    // Return the plugin version
    //
    const char SHARED_EXPORT *getPluginVersion()
    {
        return "0.0.1";
    }

    bool SHARED_EXPORT open(void *handler, char *filename, unsigned int threads)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->open(filename, threads);
    }

    bool SHARED_EXPORT close(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->close();
    }

    bool SHARED_EXPORT seek(void *handler, unsigned long long position, unsigned int mode)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->seek(position, mode);
    }

    bool SHARED_EXPORT seekCurrentDisk(void *handler, unsigned long long position, unsigned int mode)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->seekCurrentDisk(position, mode);
    }

    unsigned long long SHARED_EXPORT tell(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->tell();
    }

    unsigned long long SHARED_EXPORT tellCurrentDisk(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->tellCurrentDisk();
    }

    char SHARED_EXPORT *getID(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getID();
    }

    char SHARED_EXPORT *getDiskID(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getDiskID();
    }

    // ISO Images doesn't have any information about title
    char SHARED_EXPORT *getTitle(void *handler)
    {
        return NULL;
    }

    char SHARED_EXPORT *getDiskTitle(void *handler)
    {
        return NULL;
    }

    bool SHARED_EXPORT isOK(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->isOK();
    }

    char SHARED_EXPORT *getError(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getError();
    }

    void SHARED_EXPORT clearError(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        object->clearError();
    }

    unsigned int SHARED_EXPORT getTotalDisks(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getTotalDisks();
    }

    unsigned int SHARED_EXPORT getCurrentDisk(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getCurrentDisk();
    }

    bool SHARED_EXPORT changeCurrentDisk(void *handler, unsigned int disk)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->changeCurrentDisk(disk);
    }

    unsigned long long SHARED_EXPORT readData(void *handler, char *output, unsigned long long toRead)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->readData(output, toRead);
    }

    const char SHARED_EXPORT *getCompatibleExtensions(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getCompatibleExtensions();
    }
}