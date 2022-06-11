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

    auto plugins = load_plugins("C:/Users/danix/Desktop/PopStation MD Info/popstationmdg_plugins/popstationmdg_plugins_iso/bin/windows", EXT);
    for (auto ph : plugins)
    {
        fprintf(stderr, "Loading plugin...\n");
        auto plugin = ph->load();
        if (plugin == NULL)
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

        // Open a file to get the data
        fprintf(stderr, "Opening the test file\n");
        bool opening = ph->open("C:\\Users\\danix\\Desktop\\aa\\Firebugs (Europe)1.bin");
        if (!opening)
        {
            fprintf(stderr, "Error opening: %s\n", ph->getError().c_str());
        }

        // Get the game ID
        fprintf(stderr, "Getting the ID\n");
        std::string id_test = ph->getDiskID();
        if (id_test.c_str() != "")
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
        }
        else
        {
            fprintf(stderr, "%s\n", id_test.c_str());
        }

        fprintf(stderr, "Closing the test file\n");
        bool closing = ph->close();
        if (closing)
        {
            fprintf(stderr, "Error: %s\n", ph->getError().c_str());
        }
    }

    return 0;
}