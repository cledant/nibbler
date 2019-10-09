#include <iostream>

#include "World.hpp"

int
main()
{
    std::cout << "TODO : args parsing" << std::endl;

    try {
        WorldParams params = { 30,         30,         GFX_GLFW,
                               SOUND_NONE, ONE_PLAYER, getenv("HOME") };
        World world(std::move(params));
        world.run();
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
    return (0);
}