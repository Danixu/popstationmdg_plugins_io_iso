#include "iso_reader.hpp"

IsoReader::IsoReader() {}
IsoReader::~IsoReader() {}

// Open the ISO file
bool IsoReader::open(std::string fileName, uint8_t threads)
{
    // This reader is very simple and non CPU intensive, so threads will be ignored

    // Open source file
    input_file.open(fileName, std::ios::in | std::ios::binary);
    if (!input_file.is_open())
    {
        setLastError("There was an error opening the input file");
        return false;
    }
    return true;
}

// Close the ISO file (if was opened)
bool IsoReader::close()
{
    if (input_file.is_open())
    {
        input_file.close();
        if (!input_file.is_open())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;

    // After testing the getID function, this must be uncommented
    // game_id = "";
}

// Seek into the file
bool IsoReader::seek(size_t position, PluginSeekMode mode)
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
        setLastError("There was an error seeking into the file");
        return false;
    }
    return true;
}

// Same as above because ISO is just single disk format
bool IsoReader::seekCurrentDisk(size_t position, PluginSeekMode mode)
{
    return seek(position, mode);
}

// Get the current image position
size_t IsoReader::tell()
{
    if (input_file.is_open())
    {
        return input_file.tellg();
    }
    else
    {
        setLastError("There is no file opened");
        return 0;
    }
}

// Get the current image position
size_t IsoReader::tellCurrentDisk()
{
    return tell();
}

// Get the disk ID (currently a dummy)
std::string IsoReader::getID()
{
    if (game_id != "")
    {
        return game_id;
    }
    else
    {
        // GameID detection code (TO-DO)
        return "";
    }
}

// In ISO files the ID and DiskID are the same (is just a disk)
std::string IsoReader::getDiskID()
{
    return getID();
}

// Check if Status is OK
bool IsoReader::isOK()
{
    return is_ok;
}

// Get the last error
std::string IsoReader::getError()
{
    return last_error;
}

// Clear the last error and isOK status
void IsoReader::clearError()
{
    last_error = "";
    is_ok = true;
}

// Get total disk will be always 1 if a disk is oppened
uint8_t IsoReader::getTotalDisks()
{
    if (input_file.is_open())
    {
        return 1;
    }
    else
    {
        setLastError("There is no file opened");
        return 0;
    }
}

// Same as above
uint8_t IsoReader::getCurrentDisk()
{
    if (input_file.is_open())
    {
        return 1;
    }
    else
    {
        setLastError("There is no file opened");
        return 0;
    }
}

// ChangeCurrentDisk is not available for this format.
bool IsoReader::changeCurrentDisk(uint8_t disk)
{
    return false;
}

// Read the input file data into the provided buffer. Return the readed bytes.
size_t IsoReader::read(uint8_t *output, size_t toRead)
{
    if (!input_file.is_open())
    {
        // There is no open file
        setLastError("There is no file opened");
        return 0;
    }

    if (input_file.read(output, toRead))
    {
        return input_file.gcount();
    }
    else
    {
        if (input_file.fail())
        {
            setLastError(std::strerror(errno));
            return input_file.gcount();
        }
        else
        {
            if (input_file.eof() && input_file.gcount() == 0)
            {
                setLastError("File is already at End Of File");
                return 0;
            }
            else
            {
                return input_file.gcount();
            }
        }
    }
}

// Return the compatible extensions.
std::vector<std::string> IsoReader::getCompatibleExtensions()
{
    return extensions;
}

// Set the last error text and isOK to false
void IsoReader::setLastError(std::string error)
{
    last_error = error;
    is_ok = false;
}
