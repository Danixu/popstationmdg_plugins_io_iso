#include "iso.hpp"

// Read the input file data into the provided buffer. Return the readed bytes.
unsigned long long IsoReader::writeData(char *input, unsigned long long inputSize)
{
    if (!output_file.is_open())
    {
        // There is no opened file
        if (log != nullptr)
            log->error("There is no output file opened.", __METHOD_NAME__);
        setLastError(std::string("There is no output file opened"));
        return 0;
    }

    // Try to read from file
    try
    {
        if (log != nullptr)
            log->trace(std::string("Writing ").append(std::to_string(inputSize)).append(" bytes to output file."), __METHOD_NAME__);
        output_file.write(input, inputSize);
        return inputSize; // If nothing has failed, then all the data was writen.
    }
    catch (std::ios_base::failure &e)
    {
        if (log != nullptr)
            log->error(std::string("There was an error reading from the file: ").append(e.what()), __METHOD_NAME__);
        setLastError(std::string("There was an error reading from the file: ").append(e.what()));
        return 0;
    }
}

void IsoReader::freeWriterResources()
{
    if (log != nullptr)
        log->debug("Freeing the writer resources", __METHOD_NAME__);

    if (log != nullptr)
        log->debug("Done.", __METHOD_NAME__);
}

extern "C"
{
    // This format doesn't support multidisk feature
    bool SHARED_EXPORT hasMultiDiskSupport()
    {
        return false;
    }

    unsigned int SHARED_EXPORT getMaxDisks()
    {
        return 1;
    }

    bool SHARED_EXPORT setTotalDisks(void *handler, unsigned int totalDisks)
    {
        return true;
    }

    unsigned long long SHARED_EXPORT writeData(void *handler, char *input, unsigned long long inputSize)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->writeData(input, inputSize);
    }

    bool SHARED_EXPORT setGameID(void *handler, char *gameID)
    {
        return true;
    }

    bool SHARED_EXPORT setDiskID(void *handler, char *gameID)
    {
        return true;
    }

    // Reserved for MultiDisk formats
    bool SHARED_EXPORT addNewDisk(void *handler)
    {
        return true;
    }

    // Reserved for MultiDisk formats
    bool SHARED_EXPORT closeCurrentDisk(void *handler)
    {
        return true;
    }
}