#include "iso.h"
#define DEBUG 1

namespace PopstationmdgPlugin
{
    // Reader constructor
    IsoReader::IsoReader()
    {
        // Initialize log system
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] [thread %t] %v");
        spdlog::flush_every(std::chrono::seconds(1));

#ifdef DEBUG
        spdlog::set_level(spdlog::level::debug);
#endif

        spdlog::info("Starting the ISO plugin");

        // Set the input stream to throw an exception is failbit or badbit was set
        std::ios_base::iostate inputExceptionMask = input_file.exceptions() | std::ifstream::failbit | std::ifstream::badbit;
        input_file.exceptions(inputExceptionMask);

        // Set the output stream to throw an exception is failbit or badbit was set
        std::ios_base::iostate outputExceptionMask = output_file.exceptions() | std::ifstream::failbit | std::ifstream::badbit;
        output_file.exceptions(outputExceptionMask);
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
    bool IsoReader::open(char *filename, unsigned int mode, unsigned int compressionLevel, unsigned int threads)
    {
        // This plugin is very simple and non CPU intensive, so threads are not required and will be ignored

        // Set the plugin mode
        pluginMode = (PluginType)mode;

        if (pluginMode & PTWriter)
        {
            // New file, new life. Reset all past errors.
            output_file.clear();

            // Open the destination file
            try
            {
                spdlog::debug("ISO: Openning the output file: {}", filename);
                output_file.open(filename, std::ifstream::binary);
                spdlog::debug("ISO: File opened correctly");
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
            // New file, new life. Reset all past errors.
            input_file.clear();

            // Open source file
            try
            {
                input_file.open(filename, std::ifstream::binary);
                // Seek to the end to get the size
                seek(0, PluginSeekMode_End);
                // Get the disk size
                diskSize = tell();
                diskRealSize = diskSize;
                // Return to the begin
                seek(0, PluginSeekMode_Begin);

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
        if (gameID != nullptr)
        {
            spdlog::debug("Clearing the gameID object");
            delete[] gameID;
            gameID = nullptr;
        }

        // Try to close the input and output files
        if (input_file.is_open())
        {
            try
            {

                spdlog::debug("Closing the input file");
                input_file.close();
                input_file.clear();
            }
            catch (std::ios_base::failure &e)
            {
                setLastError(std::string("There was an error closing the input file: ") + std::string(e.what()));
            }
        }

        if (output_file.is_open())
        {
            try
            {

                spdlog::debug("Closing the output file");
                output_file.close();
                output_file.clear();
            }
            catch (std::ios_base::failure &e)
            {
                setLastError(std::string("There was an error closing the output file: ") + std::string(e.what()));
            }
        }
        spdlog::debug("Everything was closed correctly");

        return true;
    }

    unsigned long long IsoReader::getDiskSize()
    {
        return diskSize;
    }

    unsigned long long IsoReader::getDiskRealSize()
    {
        return diskRealSize;
    }

    // Seek into the file
    bool IsoReader::seek(unsigned long long position, unsigned int mode)
    {
        if (pluginMode & PTWriter)
        {
            if (!output_file.is_open())
            {
                setLastError(std::string("There is no output file opened"));
                return false;
            }
        }
        else
        {
            if (!input_file.is_open())
            {
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
                setLastError(std::string("Error seeking into the file: Tried to backward below the 0 position."));
                return false;
            }
            else
            {
                position = tell() - position;
            }
        }

        if (pluginMode & PTWriter)
        {
            try
            {
                if (!output_file.seekp(position, seek_mode))
                {
                    setLastError(std::string("There was an error seeking into the output file"));
                    return false;
                }
            }
            catch (std::ios_base::failure &e)
            {
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
                    setLastError(std::string("There was an error seeking into the input file"));
                    return false;
                }
            }
            catch (std::ios_base::failure &e)
            {
                setLastError(std::string("There was an error seeking into the input file: ") + std::string(e.what()));
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
                try
                {
                    return output_file.tellp();
                }
                catch (std::ios_base::failure &e)
                {
                    setLastError(std::string("There was an error getting the current output file position: ").append(e.what()));
                    return false;
                }
            }
            else
            {
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
                    setLastError(std::string("There was an error getting the current input file position: ").append(e.what()));
                    return false;
                }
            }
            else
            {
                setLastError(std::string("There is no input file opened."));
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
        if (last_error != nullptr)
        {
            size_t error_size = strlen(last_error);
            if (error_size > buffersize)
            {
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
        if (last_error != nullptr)
        {
            delete[] last_error;
            last_error = nullptr;
        }

        isOk = true;
    }

    void IsoReader::setLastError(std::string error)
    {
        spdlog::error("ISO: {}", error);
        size_t value_length = error.length() + 1;
        // If string is not empty
        if (value_length > 1)
        {
            if (last_error != nullptr)
            {
                delete[] last_error;
            }

            last_error = new char[value_length];
            memset(last_error, 0, value_length);

            strncpy_s(last_error, value_length, error.c_str(), error.length());
            isOk = false;
        }
    }

    // Set the last error text and isOK to false
    void IsoReader::setLastError(char *error)
    {
        if (error != nullptr)
        {
            if (last_error != nullptr)
            {
                delete[] last_error;
            }

            last_error = error;
            isOk = false;
        }
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
                setLastError(std::string("There is no output file opened."));
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
                return 1;
            }
            else
            {
                setLastError(std::string("There is no output file opened."));
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
                setLastError(std::string("There is no input file opened."));
                return 0;
            }
        }
    }

    bool IsoReader::setSettings(const char *settingsData, unsigned long settingsSize)
    {
        json settings = json::parse(settingsData);

        if (settings.contains("enable_buffer"))
        {
            bufferEnabled = settings["enable_buffer"];
        }

        if (settings.contains("buffer_size"))
        {
            bufferSize = settings["buffer_size"];
        }

        return true;
    }

    extern "C"
    {
        //
        // Get all the plugin info in json format.
        //
        void SHARED_EXPORT getPluginInfo(char *output, unsigned long long &buffersize)
        {
            ordered_json pluginInfo = ordered_json::parse(R"""({
                "info" : {
                    "name" : "ISO Image",
                    "version" : "0.5.0",
                    "description" : "This plugin will write the data into ISO format. ISO is a raw format, so it doesn't have any kind of compression, multidisk or personalization support.",
                    "multidisk" : false,
                    "maxdisks" : 1,
                    "compatibleExtensions" : [
                        "iso"
                    ],
                    "customAppearance" : false,
                    "type" : )""" + std::to_string(PTReader | PTWriter) +
                                                          R"""(
                },
                "settings" : {
                    "Reader" : {
                        "enable_buffer" : {
                            "type" : "checkbox",
                            "description" : "Enable read buffer",
                            "tooltip" : "Enable a buffer memory to speed up the read process",
                            "default" : false
                        },
                        "buffer_size" : {
                            "type" : "spin",
                            "description" : "Read buffer size",
                            "tooltip" : "Set the amount of memory reserved for the read buffer",
                            "minvalue" : )""" + std::to_string(SETTINGS_MIN_BUFFER) +
                                                          R"""(,
                            "maxvalue" : )""" + std::to_string(SETTINGS_MAX_BUFFER) +
                                                          R"""(,
                            "default" : )""" + std::to_string(SETTINGS_DEFAULT_BUFFER) +
                                                          R"""(
                        }
                    },
                    "Writer" : {
                        "enable_buffer" : {
                            "type" : "checkbox",
                            "description" : "Enable write buffer",
                            "tooltip" : "Enable a buffer memory to speed up the write process",
                            "default" : false
                        },
                        "buffer_size" : {
                            "type" : "spin",
                            "description" : "Write buffer size",
                            "tooltip" : "Set the amount of memory reserved for the write buffer",
                            "minvalue" : )""" + std::to_string(SETTINGS_MIN_BUFFER) +
                                                          R"""(,
                            "maxvalue" : )""" + std::to_string(SETTINGS_MAX_BUFFER) +
                                                          R"""(,
                            "default" : )""" + std::to_string(SETTINGS_DEFAULT_BUFFER) +
                                                          R"""(
                        }
                    }
                }
            })""");

            std::string settingsDataStr = pluginInfo.dump();

            if (settingsDataStr.size() > buffersize)
            {
                spdlog::error("The settings output buffer is not enough.");
            }

            // Clear the output buffer
            memset(output, 0, buffersize);

            // Copy the plugin info to the output buffer
            strncpy_s(output, buffersize, settingsDataStr.c_str(), settingsDataStr.size());

            // Update the exported size
            buffersize = settingsDataStr.size();
        }

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

        bool SHARED_EXPORT open(void *handler, char *filename, unsigned int mode = PTReader, unsigned int compression = 9, unsigned int threads = 1)
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

        unsigned long long SHARED_EXPORT getDiskSize(void *handler)
        {
            IsoReader *object = (IsoReader *)handler;

            return object->getDiskSize();
        }

        unsigned long long SHARED_EXPORT getDiskRealSize(void *handler)
        {
            IsoReader *object = (IsoReader *)handler;

            return object->getDiskRealSize();
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

        bool SHARED_EXPORT setSettings(void *handler, const char *settingsData, unsigned long settingsSize)
        {
            IsoReader *object = (IsoReader *)handler;

            return object->setSettings(settingsData, settingsSize);
        }
    }
}