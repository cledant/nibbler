#include <iostream>

#include "SFMLGraphic.hpp"

void
SFMLGraphic::init()
{
    std::cout << "HI THIS IS SFML LIB" << std::endl;
}

extern "C" IGraphic *
creator()
{
    return (new SFMLGraphic());
}

extern "C" void
deleter(IGraphic *gfx)
{
    delete gfx;
}