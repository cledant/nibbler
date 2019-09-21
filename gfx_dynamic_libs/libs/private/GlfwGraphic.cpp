#include <iostream>

#include "GlfwGraphic.hpp"

void
GlfwGraphic::init()
{
    std::cout << "HI THIS IS GLFW LIB" << std::endl;
}

extern "C" IGraphic *
creator()
{
    return (new GlfwGraphic());
}

extern "C" void
deleter(IGraphic *gfx)
{
    delete gfx;
}