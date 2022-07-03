#include "iso.hpp"

// Reader constructor
IsoReader::IsoReader()
{
    // Set the input stream to throw an exception is failbit or badbit was set
    std::ios_base::iostate inputExceptionMask = input_file.exceptions() | std::ifstream::failbit | std::ifstream::badbit;
    input_file.exceptions(inputExceptionMask);

    // Set the output stream to throw an exception is failbit or badbit was set
    std::ios_base::iostate outputExceptionMask = output_file.exceptions() | std::ifstream::failbit | std::ifstream::badbit;
    output_file.exceptions(outputExceptionMask);

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
    // Free the resources
    if (pluginMode & PTWriter)
    {
        freeWriterResources();
    }
    else
    {
        freeReaderResources();
    }

    // Close the file
    close();

    // Clear the last error data
    clearError();
}

// Open the ISO file
bool IsoReader::open(char *filename, unsigned int mode, unsigned int threads)
{
    // This writer is very simple and non CPU intensive, so threads are not required and will be ignored

    // Set the writer mode
    pluginMode = (PluginType)mode;

    if (pluginMode & PTWriter)
    {
        // Open the destination file
        try
        {
            output_file.open(filename, std::ifstream::binary);
            return true;
        }
        catch (std::ios_base::failure &e)
        {
            setLastError(std::string("There was an error opening the file: ") + std::string(e.what()));
            return false;
        }
    }
    else if (pluginMode & PTReader)
    {
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

    return false;
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

    // Try to close the input and output files
    try
    {
        if (input_file.is_open())
        {
            input_file.close();
        }

        return true;
    }
    catch (std::ios_base::failure &e)
    {
        setLastError(std::string("There was an error closing the input file: ") + std::string(e.what()));
        return false;
    }

    try
    {
        if (output_file.is_open())
        {
            output_file.close();
        }

        return true;
    }
    catch (std::ios_base::failure &e)
    {
        setLastError(std::string("There was an error closing the output file: ") + std::string(e.what()));
        return false;
    }
}

// Seek into the file
bool IsoReader::seek(unsigned long long position, unsigned int mode)
{
    if (pluginMode & PTWriter)
    {
        if (!output_file.is_open())
        {
            setLastError(std::string("There is no file opened"));
            return 0;
        }
    }
    else
    {
        if (!input_file.is_open())
        {
            setLastError(std::string("There is no file opened"));
            return 0;
        }
    }

    auto seek_mode = std::ios::beg;

    if (mode == PluginSeekMode_End)
    {
        seek_mode = std::ios::end;
    }
    else if (mode == PluginSeekMode_Forward)
    {
        if (pluginMode & PTWriter)
        {
            position += output_file.tellp();
        }
        else
        {
            position += input_file.tellg();
        }
    }
    else if (mode == PluginSeekMode_Backward)
    {
        if (pluginMode & PTWriter)
        {
            position = uint64_t(output_file.tellp()) - position;
        }
        else
        {
            position = uint64_t(input_file.tellg()) - position;
        }
    }

    if (pluginMode & PTWriter)
    {
        if (!output_file.seekp(position, seek_mode))
        {
            setLastError(std::string("There was an error seeking into the file"));
            return false;
        }
    }
    else
    {
        try
        {
            if (!input_file.seekg(position, seek_mode))
            {
                setLastError(std::string("There was an error seeking into the file"));
                return false;
            }
        }
        catch (std::ios_base::failure &e)
        {
            setLastError(std::string("There was an error seeking into the file: ") + std::string(e.what()));
            return false;
        }
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
    if (pluginMode & PTWriter)
    {
        if (output_file.is_open())
        {
            return output_file.tellp();
        }
        else
        {
            setLastError(std::string("There is no file opened"));
            return 0;
        }
    }
    else
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
}

// Get the current image position
unsigned long long IsoReader::tellCurrentDisk()
{
    return tell();
}

// Check if Status is OK
bool IsoReader::isOK()
{
    return isOk;
}

// Get the last error
bool IsoReader::getError(char *error, unsigned long long buffersize)
{
    // Fill the error buffer with zeroes
    memset(error, 0, buffersize);
    if (last_error != NULL)
    {
        size_t error_size = strlen(last_error);
        if (error_size > buffersize)
        {
            fprintf(stderr, "The output buffer size is too small\n");
            return false;
        }

        strncpy_s(error, buffersize, last_error, error_size);
        return true;
    }
    else
    {
        return true;
    }
}

// Clear the last error and isOK status
void IsoReader::clearError()
{
    if (last_error != NULL)
    {
        delete[] last_error;
        last_error = NULL;
    }

    isOk = true;
}

void IsoReader::setLastError(std::string error)
{
    size_t value_length = error.length() + 1;
    // If string is not empty
    if (value_length > 1)
    {
        if (last_error != NULL)
        {
            delete[] last_error;
        }

        last_error = new char[value_length];
        memset(last_error, 0, value_length);

        strncpy_s(last_error, value_length, error.c_str(), error.length());
    }
}

// Set the last error text and isOK to false
void IsoReader::setLastError(char *error)
{
    if (error != NULL)
    {
        if (last_error != NULL)
        {
            delete[] last_error;
        }

        last_error = error;
        isOk = false;
    }
    fprintf(stderr, "ISO ERROR: %s\n", last_error);
}

unsigned int IsoReader::getCurrentDisk()
{
    if (pluginMode & PTWriter)
    {
        if (output_file.is_open())
        {
            return 1;
        }
        else
        {
            setLastError(std::string("There is no file opened"));
            return 0;
        }
    }
    else
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
}

unsigned int IsoReader::getTotalDisks()
{
    if (pluginMode & PTWriter)
    {
        if (output_file.is_open())
        {
            return 1;
        }
        else
        {
            setLastError(std::string("There is no file opened"));
            return 0;
        }
    }
    else
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
}

extern "C"
{
    //
    // Creates a new plugin object in memory and return its address
    //
    void SHARED_EXPORT *load()
    {
        void *ptr = (void *)new IsoReader();
        return ptr;
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
        return PTReader | PTWriter;
    }

    //
    // Return the plugin name
    //
    bool SHARED_EXPORT getPluginName(char *name, unsigned long long buffersize)
    {
        // Compatible extensions for the reader/writter. Use pipe "|" between the extension: "*.iso|*.bin"
        const char pn[] = "ISO Image";

        if (sizeof(pn) > buffersize)
        {
            return false;
        }

        strncpy_s(name, buffersize, pn, sizeof(pn));
        return true;
    }

    //
    // Return the plugin version
    //
    bool SHARED_EXPORT getPluginVersion(char *version, unsigned long long buffersize)
    {
        // Compatible extensions for the reader/writter. Use pipe "|" between the extension: "*.iso|*.bin"
        const char pv[] = "0.0.1";

        if (sizeof(pv) > buffersize)
        {
            return false;
        }

        strncpy_s(version, buffersize, pv, sizeof(pv));
        return true;
    }

    bool SHARED_EXPORT open(void *handler, char *filename, unsigned int mode, unsigned int threads)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->open(filename, mode, threads);
    }

    bool SHARED_EXPORT close(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->close();
    }

    bool SHARED_EXPORT isOK(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->isOK();
    }

    bool SHARED_EXPORT getError(void *handler, char *error, unsigned long long buffersize)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getError(error, buffersize);
    }

    void SHARED_EXPORT clearError(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        object->clearError();
    }

    bool SHARED_EXPORT getCompatibleExtensions(char *extensions, unsigned long long buffersize)
    {
        // Compatible extensions for the reader/writter. Use pipe "|" between the extension: "*.iso|*.bin"
        const char ext[] = "iso";

        if (sizeof(ext) > buffersize)
        {
            return false;
        }

        strncpy_s(extensions, buffersize, ext, sizeof(ext));
        return true;
    }

    unsigned int SHARED_EXPORT getCurrentDisk(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getCurrentDisk();
    }

    unsigned int SHARED_EXPORT getTotalDisks(void *handler)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getTotalDisks();
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
}