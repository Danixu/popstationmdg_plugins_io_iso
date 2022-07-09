#include "iso.hpp"

// Reader constructor
IsoReader::IsoReader(void *logger)
{
    // Logger
    if (logger != nullptr)
    {
        log = (Logging::Logger *)logger;
        log->debug("Initializing the ISO Image plugin.", __METHOD_NAME__);
    }

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
    if (log != nullptr)
        log->debug("Deleting ISO Image plugin object.", __METHOD_NAME__);
    // Free the resources
    if (pluginMode & PTWriter)
    {
        if (log != nullptr)
            log->debug("Plugin is in writer mode. Freeing writter resources.", __METHOD_NAME__);
        freeWriterResources();
    }
    else
    {
        if (log != nullptr)
            log->debug("Plugin is in reader mode. Freeing writter resources.", __METHOD_NAME__);
        freeReaderResources();
    }

    // Close the file
    if (log != nullptr)
        log->debug("Closing the plugin.", __METHOD_NAME__);
    close();

    // Clear the last error data
    if (log != nullptr)
        log->debug("Clearing error data", __METHOD_NAME__);
    clearError();
    if (log != nullptr)
        log->debug("Object is ready to be deleted.", __METHOD_NAME__);
}

// Open the ISO file
bool IsoReader::open(char *filename, unsigned int mode, unsigned int threads)
{
    // This plugin is very simple and non CPU intensive, so threads are not required and will be ignored

    // Set the plugin mode
    pluginMode = (PluginType)mode;

    if (pluginMode & PTWriter)
    {
        if (log != nullptr)
            log->debug("Plugin is in writer mode. Opening the file.", __METHOD_NAME__);
        // Open the destination file
        try
        {
            output_file.open(filename, std::ifstream::binary);
            if (log != nullptr)
                log->debug("File opened correctly.", __METHOD_NAME__);
            return true;
        }
        catch (std::ios_base::failure &e)
        {
            if (log != nullptr)
                log->debug(std::string("There was an error opening the file: ").append(e.what()), __METHOD_NAME__);
            setLastError(std::string("There was an error opening the file: ") + std::string(e.what()));
            return false;
        }
    }
    else if (pluginMode & PTReader)
    {
        // Open source file
        if (log != nullptr)
            log->debug("Plugin is in reader mode. Opening the file.", __METHOD_NAME__);
        try
        {
            input_file.open(filename, std::ifstream::binary);
            if (log != nullptr)
                log->debug("File opened correctly.", __METHOD_NAME__);
            return true;
        }
        catch (std::ios_base::failure &e)
        {
            if (log != nullptr)
                log->debug(std::string("There was an error opening the file: ").append(e.what()), __METHOD_NAME__);
            setLastError(std::string("There was an error opening the file: ") + std::string(e.what()));
            return false;
        }
    }

    return false;
}

// Close the ISO file (if was opened)
bool IsoReader::close()
{
    if (log != nullptr)
        log->debug("Closing the file...", __METHOD_NAME__);
    // Delete the ID which is not usefull anymore
    if (gameID != nullptr)
    {
        if (log != nullptr)
            log->debug("Clearing the gameID variable.", __METHOD_NAME__);

        delete[] gameID;
        gameID = nullptr;

        if (log != nullptr)
            log->debug("gameID variable cleared.", __METHOD_NAME__);
    }

    if (log != nullptr)
        log->debug("Trying to close the input/output file.", __METHOD_NAME__);
    // Try to close the input and output files
    try
    {
        if (input_file.is_open())
        {
            if (log != nullptr)
                log->debug("Closing the intput file.", __METHOD_NAME__);

            input_file.close();

            if (log != nullptr)
                log->debug("Input file closed correctly.", __METHOD_NAME__);
        }

        return true;
    }
    catch (std::ios_base::failure &e)
    {
        if (log != nullptr)
            log->error(std::string("There was an error closing the input file: ").append(e.what()), __METHOD_NAME__);
        setLastError(std::string("There was an error closing the input file: ") + std::string(e.what()));
        return false;
    }

    try
    {
        if (output_file.is_open())
        {
            if (log != nullptr)
                log->debug("Closing the output file.", __METHOD_NAME__);

            output_file.close();

            if (log != nullptr)
                log->debug("Output file closed correctly.", __METHOD_NAME__);
        }

        return true;
    }
    catch (std::ios_base::failure &e)
    {
        if (log != nullptr)
            log->error(std::string("There was an error closing the output file: ").append(e.what()), __METHOD_NAME__);
        setLastError(std::string("There was an error closing the output file: ") + std::string(e.what()));
        return false;
    }
}

// Seek into the file
bool IsoReader::seek(unsigned long long position, unsigned int mode)
{
    if (log != nullptr)
    {
        std::string logmsg = "Seeking the position to: ";
        logmsg.append(std::to_string(position));
        logmsg.append(" - using mode: ");
        logmsg.append(std::to_string(mode));
        log->debug(logmsg, __METHOD_NAME__);
    }

    if (pluginMode & PTWriter)
    {
        if (!output_file.is_open())
        {
            if (log != nullptr)
                log->error("There is no output file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no file opened"));
            return false;
        }
    }
    else
    {
        if (!input_file.is_open())
        {
            if (log != nullptr)
                log->error("There is no input file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no file opened"));
            return false;
        }
    }

    auto seek_mode = std::ios::beg;

    if (mode == PluginSeekMode_End)
    {
        seek_mode = std::ios::end;
    }
    else if (mode == PluginSeekMode_Forward)
    {
        position += tell();
    }
    else if (mode == PluginSeekMode_Backward)
    {
        // If you want to backward more than the file current position
        if (tell() < position)
        {
            if (log != nullptr)
                log->error("Error seeking into the file: Tried to backward below the 0 position.", __METHOD_NAME__);
            setLastError(std::string("Error seeking into the file: Tried to backward below the 0 position."));
            return false;
        }
        else
        {
            position = tell() - position;
        }
    }

    if (log != nullptr)
    {
        std::string logmsg = "Current position: ";
        logmsg.append(std::to_string(tell()));
        logmsg.append(" - New file position: ");
        logmsg.append(std::to_string(position));
        logmsg.append(" - mode: ");
        logmsg.append(std::to_string(mode));
        log->debug(logmsg, __METHOD_NAME__);
    }

    if (pluginMode & PTWriter)
    {
        try
        {
            if (!output_file.seekp(position, seek_mode))
            {
                if (log != nullptr)
                    log->error("There was an error seeking into the output file.", __METHOD_NAME__);
                setLastError(std::string("There was an error seeking into the output file"));
                return false;
            }
        }
        catch (std::ios_base::failure &e)
        {
            if (log != nullptr)
                log->error("There was an error seeking into the output file.", __METHOD_NAME__);
            setLastError(std::string("There was an error seeking into the output file: ") + std::string(e.what()));
            return false;
        }
    }
    else
    {
        try
        {
            if (!input_file.seekg(position, seek_mode))
            {
                if (log != nullptr)
                    log->error("There was an error seeking into the input file.", __METHOD_NAME__);
                setLastError(std::string("There was an error seeking into the input file"));
                return false;
            }
        }
        catch (std::ios_base::failure &e)
        {
            if (log != nullptr)
                log->error("There was an error seeking into the input file.", __METHOD_NAME__);
            setLastError(std::string("There was an error seeking into the input file: ") + std::string(e.what()));
            return false;
        }
    }

    if (log != nullptr)
        log->debug("Seeked correctly.", __METHOD_NAME__);

    return true;
}

// Same as above because ISO is just single disk format
bool IsoReader::seekCurrentDisk(unsigned long long position, unsigned int mode)
{
    if (log != nullptr)
        log->debug("Calling seek method.", __METHOD_NAME__);
    return seek(position, mode);
}

// Get the current image position
unsigned long long IsoReader::tell()
{
    if (log != nullptr)
        log->debug("Getting the file current position (tell).", __METHOD_NAME__);

    if (pluginMode & PTWriter)
    {
        if (output_file.is_open())
        {
            try
            {
                return output_file.tellp();
            }
            catch (std::ios_base::failure &e)
            {
                if (log != nullptr)
                    log->error(std::string("There was an error getting the current output file position: ").append(e.what()), __METHOD_NAME__);
                setLastError(std::string("There was an error getting the current output file position: ").append(e.what()));
                return false;
            }
        }
        else
        {
            if (log != nullptr)
                log->error("There is no output file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no output file opened."));
            return 0;
        }
    }
    else
    {
        if (input_file.is_open())
        {
            try
            {
                return input_file.tellg();
            }
            catch (std::ios_base::failure &e)
            {
                if (log != nullptr)
                    log->error(std::string("There was an error getting the current input file position: ").append(e.what()), __METHOD_NAME__);
                setLastError(std::string("There was an error getting the current input file position: ").append(e.what()));
                return false;
            }
        }
        else
        {
            if (log != nullptr)
                log->error("There is no input file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no input file opened."));
            return 0;
        }
    }
}

// Get the current image position
unsigned long long IsoReader::tellCurrentDisk()
{
    if (log != nullptr)
        log->debug("Calling tell method.", __METHOD_NAME__);
    return tell();
}

// Check if Status is OK
bool IsoReader::isOK()
{
    if (log != nullptr)
        log->debug("Returning plugin status (isOK).", __METHOD_NAME__);
    return isOk;
}

// Get the last error
bool IsoReader::getError(char *error, unsigned long long buffersize)
{
    // Fill the error buffer with zeroes
    if (log != nullptr)
        log->debug("Getting the last error.", __METHOD_NAME__);
    memset(error, 0, buffersize);
    if (last_error != nullptr)
    {
        size_t error_size = strlen(last_error);
        if (error_size > buffersize)
        {
            if (log != nullptr)
                log->error("The output buffer size is too small.", __METHOD_NAME__);
            return false;
        }

        strncpy_s(error, buffersize, last_error, error_size);
        if (log != nullptr)
            log->debug("The error msg was copied correctly.", __METHOD_NAME__);
        return true;
    }
    else
    {
        if (log != nullptr)
            log->debug("There is no error message.", __METHOD_NAME__);
        return true;
    }
}

// Clear the last error and isOK status
void IsoReader::clearError()
{
    if (log != nullptr)
        log->debug("Clearing the last error buffer.", __METHOD_NAME__);
    if (last_error != nullptr)
    {
        if (log != nullptr)
            log->debug("Deleting the char array.", __METHOD_NAME__);
        delete[] last_error;
        last_error = nullptr;
    }

    isOk = true;
    if (log != nullptr)
        log->debug("Last error buffer cleared.", __METHOD_NAME__);
}

void IsoReader::setLastError(std::string error)
{
    if (log != nullptr)
        log->debug("Setting last error value.", __METHOD_NAME__);
    size_t value_length = error.length() + 1;
    // If string is not empty
    if (value_length > 1)
    {
        if (log != nullptr)
            log->debug("Checking if an old message exists to clear it.", __METHOD_NAME__);
        if (last_error != nullptr)
        {
            if (log != nullptr)
                log->debug("Clearing the old message.", __METHOD_NAME__);
            delete[] last_error;
        }

        if (log != nullptr)
            log->debug("Reserving the new buffer space for the new message.", __METHOD_NAME__);
        last_error = new char[value_length];
        if (log != nullptr)
            log->debug("Zeroing the new buffer space.", __METHOD_NAME__);
        memset(last_error, 0, value_length);

        if (log != nullptr)
            log->debug("Copying the error message to the new buffer.", __METHOD_NAME__);
        strncpy_s(last_error, value_length, error.c_str(), error.length());
        isOk = false;
    }
    if (log != nullptr)
        log->debug("Done.", __METHOD_NAME__);
}

// Set the last error text and isOK to false
void IsoReader::setLastError(char *error)
{
    if (log != nullptr)
        log->debug("Setting last error value.", __METHOD_NAME__);
    if (error != nullptr)
    {
        if (last_error != nullptr)
        {
            if (log != nullptr)
                log->debug("Clearing the old message.", __METHOD_NAME__);
            delete[] last_error;
        }
        if (log != nullptr)
            log->debug("Setting the error message as the last_error message.", __METHOD_NAME__);

        last_error = error;
        isOk = false;
    }
    if (log != nullptr)
        log->debug("Done.", __METHOD_NAME__);
}

unsigned int IsoReader::getCurrentDisk()
{
    if (pluginMode & PTWriter)
    {
        if (output_file.is_open())
        {
            if (log != nullptr)
                log->trace("This plugin doesn't have multidisk support, so always return 1 as current disk.");
            return 1;
        }
        else
        {
            if (log != nullptr)
                log->error("There is no output file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no output file opened."));
            return 0;
        }
    }
    else
    {
        if (input_file.is_open())
        {
            if (log != nullptr)
                log->trace("This plugin doesn't have multidisk support, so always return 1 as current disk.");
            return 1;
        }
        else
        {
            if (log != nullptr)
                log->error("There is no input file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no input file opened."));
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
            if (log != nullptr)
                log->trace("This plugin doesn't have multidisk support, so always return 1 as total disks.");
            return 1;
        }
        else
        {
            if (log != nullptr)
                log->error("There is no output file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no output file opened."));
            return 0;
        }
    }
    else
    {
        if (input_file.is_open())
        {
            if (log != nullptr)
                log->trace("This plugin doesn't have multidisk support, so always return 1 as total disks.");
            return 1;
        }
        else
        {
            if (log != nullptr)
                log->error("There is no input file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no input file opened."));
            return 0;
        }
    }
}

extern "C"
{
    //
    // Creates a new plugin object in memory and return its address
    //
    void SHARED_EXPORT *load(void *logger)
    {
        void *ptr = (void *)new IsoReader(logger);
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