#include "plugin.hpp"
#include <cstring>
#include <fstream>

class SHARED_EXPORT IsoReader : public Plugin
{
public:
    IsoReader();
    virtual ~IsoReader();

    // Common
    virtual bool open(std::string filename, uint8_t threads = 1);
    virtual bool close();
    virtual bool seek(size_t position, PluginSeekMode mode);
    virtual bool seekCurrentDisk(size_t position, PluginSeekMode mode);
    virtual size_t tell();
    virtual size_t tellCurrentDisk();
    virtual std::string getID();
    virtual std::string getDiskID();
    virtual bool isOK();
    virtual std::string getError();
    virtual void clearError();

    virtual uint8_t getTotalDisks();
    virtual uint8_t getCurrentDisk();
    virtual bool changeCurrentDisk(uint8_t disk);

    // Reader
    virtual size_t read(uint8_t *output, size_t toRead);
    virtual std::vector<std::string> getCompatibleExtensions();

    // Writter (Not used here)
    virtual bool setID(std::string gameID) { return false; }
    virtual bool write(uint8_t &input, size_t toWrite) { return false; }
    virtual bool addDisk(size_t diskSize, std::string diskID) { return false; }
    virtual bool closeDisk() { return false; }

    // Internal methods and variables for plugin usage
private:
    // variables
    std::basic_ifstream<uint8_t> input_file;
    std::string game_id = "SCES00010";
    std::vector<std::string> extensions{"iso"};

    // Methods
    void setLastError(std::string error);
};

DEFINE_PLUGIN(IsoReader, PTReader, "ISO Image", "0.0.1")