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
        fprintf(stderr, "Running plugins getDiskID method:\n");
    }
    for (auto ph : plugins)
    {
        // Load the library functions
        auto plugin = ph->load();

        // Open a file to get the data
        const char *filename = "C:\\Users\\danix\\Desktop\\aa\\Firebugs (Europe).bin";
        ph->open(filename);
        std::string id_test = ph->getDiskID();
        fprintf(stderr, "Error: %s\n", ph->getError().c_str());
        fprintf(stderr, "%s\n", id_test.c_str());
        ph->close();
    }

    return 0;
}