#include <iostream>
#include <cstring>

#include "World.hpp"

static void
nibbler_usage()
{
    std::cout << "Usage: ./nibbler board_w board_h [--duel --obstacles]"
              << std::endl;
    std::cout << "\t board_w and board_h min value = "
              << IGraphicConstants::MIN_H << std::endl;
    std::cout << "\t board_w and board_h max value = "
              << IGraphicConstants::MAX_H << std::endl;
    std::cout << "\t --duel for 2 player game" << std::endl;
    std::cout << "\t --obstacles to spawn obstacles" << std::endl;
}

static int32_t
parse_int(char const *arg, int32_t min, int32_t max)
{
    int32_t size;
    try {
        size = std::stoi(arg);
    } catch (std::exception const &e) {
        throw std::runtime_error("Nibbler: Invalid argument: " +
                                 std::string(arg));
    }

    if (size < min || size > max) {
        throw std::runtime_error("Nibbler: Invalid Size: " + std::string(arg));
    }
    return (size);
}

void
parse_options(int argc, char const **argv, struct WorldParams &params)
{
    for (int32_t i = 3; i < argc; ++i) {
        if (!strcmp(argv[i], "--duel")) {
            params.game_type = TWO_PLAYER;
        } else if (!strcmp(argv[i], "--obstacles")) {
            params.obstacles = 1;
        }
    }
}

int
main(int32_t argc, char const **argv)
{
    WorldParams params = { 10, 10, GFX_GLFW, SOUND_NONE, ONE_PLAYER, 0 };

    try {
        if (argc < 3) {
            throw std::runtime_error("Nibbler: Invalid number of arguments");
        }
        params.board_w = parse_int(
          argv[1], IGraphicConstants::MIN_W, IGraphicConstants::MAX_W);
        params.board_h = parse_int(
          argv[2], IGraphicConstants::MIN_H, IGraphicConstants::MAX_H);
        if (argc > 3) {
            parse_options(argc, argv, params);
        }
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
        nibbler_usage();
        return (0);
    }

    World world(params);
    try {
        world.init();
        world.run();
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
    return (0);
}