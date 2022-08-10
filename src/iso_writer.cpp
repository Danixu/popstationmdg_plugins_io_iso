#include "iso.h"

namespace PopstationmdgPlugin
{
    // Read the input file data into the provided buffer. Return the readed bytes.
    unsigned long long IsoReader::writeData(char *input, unsigned long long inputSize)
    {
        if (!output_file.is_open())
        {
            // There is no opened file
            setLastError(std::string("There is no output file opened"));
            return 0;
        }

        // Try to write to file
        try
        {
            spdlog::trace("Writing {} bytes to output", inputSize);
            output_file.write(input, inputSize);
            spdlog::trace("Data was writen correctly. Returning the writen size");
            return inputSize; // If nothing has failed, then all the data was writen.
        }
        catch (std::ios_base::failure &e)
        {
            setLastError(std::string("There was an error reading from the file: ").append(e.what()));
            return 0;
        }
    }

    void IsoReader::freeWriterResources()
    {
    }

    extern "C"
    {
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
}