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

    auto plugins = load_plugins("./", EXT);
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
        auto plugin = ph->load();
        std::string id_test = std::string(ph->getDiskID());
        fprintf(stderr, "%s\n", id_test.c_str());
    }

    return 0;
}