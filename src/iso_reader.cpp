#include "iso.hpp"

// Get the disk ID
//
// MUST BE IMPROVED TO DETECT ALL THE CODES
//
bool IsoReader::getID(char *id, unsigned long long buffersize)
{
    if (buffersize < 10)
    {
        if (log != nullptr)
            log->error("The output buffer size is too small.", __METHOD_NAME__);
        setLastError(std::string("The output buffer size is too small"));
        return false;
    }

    if (log != nullptr)
        log->debug("Checking if the ID was already detected.", __METHOD_NAME__);
    if (gameID == nullptr)
    {
        if (log != nullptr)
            log->debug("The ID is empty, so we need to detect it.", __METHOD_NAME__);
        // No input file
        if (!input_file.is_open())
        {
            if (log != nullptr)
                log->error("There is no input file opened.", __METHOD_NAME__);
            setLastError(std::string("There is no input file opened"));
            return false;
        }

        if (log != nullptr)
            log->debug("Getting the current position.", __METHOD_NAME__);
        // Get current position
        unsigned long long current_pos = tell();
        // There was an error
        if (log != nullptr)
            log->debug("Checking if everything is OK.", __METHOD_NAME__);
        if (!isOK())
        {
            if (log != nullptr)
                log->error("There was an error getting the current file position.", __METHOD_NAME__);
            setLastError(std::string("There was an error getting the current file position."));
            return false;
        }

        // Go to the start of the file
        if (log != nullptr)
            log->debug("Seeking to the start point.", __METHOD_NAME__);
        if (!seek(0, PluginSeekMode_Begin))
        {
            if (log != nullptr)
                log->error("There was an error seeking to the start of the file.", __METHOD_NAME__);
            setLastError(std::string("There was an error seeking to the start of the file."));
            return false;
        }

        if (log != nullptr)
            log->debug("Reserving 200kb of RAM.", __METHOD_NAME__);
        // Reserve 200k of RAM to store the disk data
        char *disk_data = (char *)std::malloc(204800);

        if (disk_data == nullptr)
        {
            if (log != nullptr)
                log->error("There was an error allocating the required memory.", __METHOD_NAME__);
            setLastError(std::string("There was an error allocating the required memory."));
            return false;
        }

        if (log != nullptr)
            log->debug("Reading the first 200kb of the image file.", __METHOD_NAME__);
        // Read the first 200k of data into the new buffer
        size_t readed = readData(disk_data, 204800);
        if (!isOK())
        {
            if (log != nullptr)
                log->error("There was an error reading the first 200kb from file.", __METHOD_NAME__);
            std::free((void *)disk_data);
            return false;
        }

        if (log != nullptr)
            log->debug("Seeking to the original file position", __METHOD_NAME__);
        // return to the last position
        if (!seek(current_pos, PluginSeekMode_Begin))
        {
            if (log != nullptr)
                log->error("There was an error seeking to the original file position.", __METHOD_NAME__);
            setLastError(std::string("There was an error seeking to the original file position."));
            std::free((void *)disk_data);
            return false;
        }

        if (log != nullptr)
            log->debug("Detecting the gameID from the readed 200kb of data.", __METHOD_NAME__);
        // Try to extract the game ID from those 200k
        bool detected = false;
        for (size_t i = 0; i < readed; i++)
        {
            // USA Codes
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'U' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'L' && disk_data[i + 2] == 'U' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'P' && disk_data[i + 2] == 'U' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'U' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'X'))
                detected = true;

            // EUR Codes
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'E' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'L' && disk_data[i + 2] == 'E' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'E' && disk_data[i + 3] == 'D'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'L' && disk_data[i + 2] == 'E' && disk_data[i + 3] == 'D'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'E' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'X'))
                detected = true;

            // Japan Codes
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'M'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'L' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'L' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'M'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'I' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'E' && disk_data[i + 1] == 'S' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'M'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'L' && disk_data[i + 2] == 'K' && disk_data[i + 3] == 'A'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'A' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'X'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'X'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'D'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'T' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'X'))
                detected = true;
            if (!detected && (disk_data[i] == 'P' && disk_data[i + 1] == 'B' && disk_data[i + 2] == 'P' && disk_data[i + 3] == 'X'))
                detected = true;
            if (!detected && (disk_data[i] == 'C' && disk_data[i + 1] == 'P' && disk_data[i + 2] == 'C' && disk_data[i + 3] == 'S'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'A' && disk_data[i + 3] == 'J'))
                detected = true;
            if (!detected && (disk_data[i] == 'S' && disk_data[i + 1] == 'C' && disk_data[i + 2] == 'Z' && disk_data[i + 3] == 'S'))
                detected = true;

            if (detected)
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
                // Stop searching
                break;
            }
        }

        // Free the buffer
        if (log != nullptr)
            log->debug("Freeing the buffer.", __METHOD_NAME__);
        std::free((void *)disk_data);
        // If nothing was found then return false
        if (gameID == nullptr)
        {
            if (log != nullptr)
                log->warning("No ID found.", __METHOD_NAME__);
            setLastError(std::string("No ID found."));
            return false;
        }
    }

    if (log != nullptr)
        log->debug(std::string("Detected the ID ").append(gameID), __METHOD_NAME__);
    strncpy_s(id, 10, gameID, 10);
    return true;
}

// In ISO files the ID and DiskID are the same (is just a disk)
bool IsoReader::getDiskID(char *id, unsigned long long buffersize)
{
    if (log != nullptr)
        log->debug("Using getID.", __METHOD_NAME__);
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
        if (log != nullptr)
            log->error("There is no input file opened.", __METHOD_NAME__);
        setLastError(std::string("There is no input file opened"));
        return 0;
    }

    // Try to read from file
    if (log != nullptr)
        log->debug(std::string("Reading ").append(std::to_string(outputSize)).append(" from input file."), __METHOD_NAME__);
    try
    {
        input_file.read(output, outputSize);
        return input_file.gcount();
    }
    catch (std::ios_base::failure &e)
    {
        if (log != nullptr)
            log->error(std::string("There was an error reading from the file: ").append(e.what()), __METHOD_NAME__);
        setLastError(std::string("There was an error reading from the file: ").append(e.what()));
        return 0;
    }
}

void IsoReader::freeReaderResources()
{
    if (log != nullptr)
        log->debug("Freeing the reader resources", __METHOD_NAME__);
    if (gameID != nullptr)
    {
        if (log != nullptr)
            log->debug("Deleting the gameID char array", __METHOD_NAME__);
        delete[] gameID;
        gameID = nullptr;
    }
    if (log != nullptr)
        log->debug("Done.", __METHOD_NAME__);
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