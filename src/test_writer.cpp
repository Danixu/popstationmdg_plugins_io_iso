/*
 *
 * Main application which will load the plugins dinamically
 *
 */

#include <vector>
#include "plugin_handler.hpp"

#ifdef _WIN32
#define EXT ".dll"
#else
#define EXT ".so"
#endif

int main()
{
    auto plugins = load_plugins("./", EXT, PTWriter);
    for (auto ph : plugins)
    {
        fprintf(stderr, "Loading plugin...\n");
        auto plugin = ph->load();
        if (plugin == nullptr)
        {
            fprintf(stderr, "The plugin is not loaded correctly\n");
            continue;
        }
        fprintf(stderr, "Plugin loaded\n");
        fprintf(stderr, "Auto loaded plugin: %s, version: %s, type: %d\n", ph->getPluginName().c_str(), ph->getPluginVersion().c_str(), ph->getType());
    }
    for (auto ph : plugins)
    {
        // Load the library functions
        fprintf(stderr, "Loading internal dll class\n");
        auto plugin = ph->load();

        // Get the known extensions
        for (auto extension : ph->getCompatibleExtensions())
        {
            fprintf(stderr, "Known extension: %s\n", extension.c_str());
        }

        // Open a file to get the data
        fprintf(stderr, "Opening the test file\n");
        bool opening = ph->open("test_write.iso", PTWriter);
        if (!opening)
        {
            fprintf(stderr, "Error opening: %s\n", ph->getError().c_str());
        }

        // Writing into the file
        fprintf(stderr, "Writing data into the file\n");
        char dataToWrite[] = "Hello my friend\0";

        if (!ph->writeData(dataToWrite, sizeof(dataToWrite)))
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
        }
        else
        {
            fprintf(stderr, "Data was writen correctly\n");
        }

        fprintf(stderr, "Closing the test file\n");
        bool closing = ph->close();
        if (!closing)
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
        }
    }

    return 0;
}