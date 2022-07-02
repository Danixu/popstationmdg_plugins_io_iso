#include "iso.hpp"

// Get the disk ID (currently a dummy)
bool IsoReader::getID(char *id, unsigned long long buffersize)
{
    if (buffersize < 10)
    {
        setLastError(std::string("The output buffer size is too small"));
        return false;
    }

    if (gameID == NULL)
    {
        // No input file
        if (!input_file.is_open())
        {
            setLastError(std::string("There is no file opened"));
            return false;
        }

        // Get current position
        unsigned long long current_pos = tell();
        // There was an error
        if (!isOK())
        {
            setLastError(std::string("There was an error getting the current file position."));
            return false;
        }

        // Go to the start of the file
        if (!seek(0, PluginSeekMode_Begin))
        {
            setLastError(std::string("There was an error seeking to the start of the file."));
            return false;
        }

        // Reserve 200k of RAM to store the disk data
        char *disk_data = (char *)std::malloc(204800);

        if (disk_data == NULL)
        {
            setLastError(std::string("There was an error allocating the required memory."));
            return false;
        }

        // Read the first 200k of data into the new buffer
        size_t readed = readData(disk_data, 204800);
        if (!isOK())
        {
            std::free((void *)disk_data);
            return false;
        }

        // return to the last position
        if (!seek(current_pos, PluginSeekMode_Begin))
        {
            setLastError(std::string("There was an error seeking to the start of the file."));
            std::free((void *)disk_data);
            return false;
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
                memcpy(gameID, disk_data + (i), 1);
                memcpy(gameID + 1, disk_data + (i + 1), 1);
                memcpy(gameID + 2, disk_data + (i + 2), 1);
                memcpy(gameID + 3, disk_data + (i + 3), 1);
                memcpy(gameID + 4, disk_data + (i + 5), 1);
                memcpy(gameID + 5, disk_data + (i + 6), 1);
                memcpy(gameID + 6, disk_data + (i + 7), 1);
                memcpy(gameID + 7, disk_data + (i + 9), 1);
                memcpy(gameID + 8, disk_data + (i + 10), 1);
            }
        }

        // Nothing was found, so we will free the buffer and return NULL
        std::free((void *)disk_data);
        if (gameID == NULL)
        {
            setLastError(std::string("No ID found."));
            return false;
        }
    }

    strncpy_s(id, 10, gameID, 10);
    return true;
}

// In ISO files the ID and DiskID are the same (is just a disk)
bool IsoReader::getDiskID(char *id, unsigned long long buffersize)
{
    return getID(id, buffersize);
}

// ChangeCurrentDisk is not available for this format.
bool IsoReader::changeCurrentDisk(unsigned int disk)
{
    return true;
}

// Read the input file data into the provided buffer. Return the readed bytes.
unsigned long long IsoReader::readData(char *output, unsigned long long outputSize)
{
    if (!input_file.is_open())
    {
        // There is no opened file
        setLastError(std::string("There is no file opened"));
        return 0;
    }

    // Try to read from file
    try
    {
        input_file.read(output, outputSize);
        return input_file.gcount();
    }
    catch (std::ios_base::failure &e)
    {
        setLastError(std::string("There was an error reading from the file: ") + std::string(e.what()));
        return 0;
    }
}

void IsoReader::freeReaderResources()
{
    if (gameID != NULL)
    {
        delete[] gameID;
        gameID = NULL;
    }
}

extern "C"
{
    unsigned long long SHARED_EXPORT readData(void *handler, char *output, unsigned long long toRead)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->readData(output, toRead);
    }

    bool SHARED_EXPORT getGameID(void *handler, char *id, unsigned long long buffersize)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getID(id, buffersize);
    }

    bool SHARED_EXPORT getDiskID(void *handler, char *id, unsigned long long buffersize)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->getDiskID(id, buffersize);
    }

    // ISO Images doesn't have any information about title. Just return true.
    bool SHARED_EXPORT getGameTitle(void *handler, char *title, unsigned long long buffersize)
    {
        return true;
    }

    bool SHARED_EXPORT getDiskTitle(void *handler, char *title, unsigned long long buffersize)
    {
        return true;
    }

    bool SHARED_EXPORT changeCurrentDisk(void *handler, unsigned int disk)
    {
        IsoReader *object = (IsoReader *)handler;

        return object->changeCurrentDisk(disk);
    }
}