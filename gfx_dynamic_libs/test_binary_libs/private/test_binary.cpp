#include <iostream>

#include "gfx_loader.hpp"

int
main()
{
    std::string home(getenv("HOME"));
    GfxLoader gfx_loader;
    IGraphic *gfx_interface = nullptr;

    if (home.empty()) {
        std::cout << "Home not set" << std::endl;
        return (0);
    }
    try {
        gfx_loader.openLib(home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so");
        gfx_interface = gfx_loader.getCreator()();
        gfx_interface->init();
        gfx_loader.getDeleter()(gfx_interface);
    } catch (std::exception const &e) {
        e.what();
        if (!gfx_interface) {
            gfx_loader.getDeleter()(gfx_interface);
        }
    }
    return (0);
}