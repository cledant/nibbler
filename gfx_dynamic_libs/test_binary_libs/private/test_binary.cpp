#include <iostream>

#include "gfx_loader.hpp"

int
main()
{
    std::cout << "Hello Test World" << std::endl;
    GfxLoader gfx_loader;

    //GLFW
    gfx_loader.openLib("./dyn_libs/libgfx_dyn_glfw.so");
    IGraphic *interface = gfx_loader.getCreator()();
    interface->init();
    gfx_loader.getDeleter()(interface);

    //SFML
    gfx_loader.openLib("./dyn_libs/libgfx_dyn_sfml.so");
    interface = gfx_loader.getCreator()();
    interface->init();
    gfx_loader.getDeleter()(interface);
    return (0);
}