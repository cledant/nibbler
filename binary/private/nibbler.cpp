#include <iostream>

#include "World.hpp"

int
main()
{
    std::cout << "TODO : args parsing" << std::endl;
    WorldParams params = {
        30, 30, GFX_GLFW, SOUND_NONE, ONE_PLAYER};
    World world(params);

    try {
        world.init();
        world.run();
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
    return (0);
}