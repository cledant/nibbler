#include <iostream>

#include "World.hpp"

static void
nibbler_usage()
{
    std::cout << "Usage: ./nibbler board_w board_h [nb_player]" << std::endl;
    std::cout << "\t board_w and board_h min value = "
              << IGraphicConstants::MIN_H << std::endl;
    std::cout << "\t board_w and board_h max value = "
              << IGraphicConstants::MAX_H << std::endl;
    std::cout << "\t nb_player is either 1 or 2" << std::endl;
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

static enum Gametype
parse_gametype(char const *arg)
{
    int32_t nb_player;
    try {
        nb_player = std::stoi(arg);
    } catch (std::exception const &e) {
        throw std::runtime_error("Nibbler: Invalid argument: " +
                                 std::string(arg));
    }

    if (nb_player == 1) {
        return (Gametype::ONE_PLAYER);
    } else if (nb_player == 2) {
        return (Gametype::TWO_PLAYER);
    } else {
        throw std::runtime_error("Nibbler: Invalid number of player: " +
                                 std::string(arg));
    }
}

int
main(int32_t argc, char const **argv)
{
    WorldParams params = { 10, 10, GFX_GLFW, SOUND_NONE, ONE_PLAYER };

    try {
        if (argc < 3 || argc > 5) {
            throw std::runtime_error("Nibbler: Invalid number of arguments");
        }
        params.board_w = parse_int(
          argv[1], IGraphicConstants::MIN_W, IGraphicConstants::MAX_W);
        params.board_h = parse_int(
          argv[2], IGraphicConstants::MIN_H, IGraphicConstants::MAX_H);
        if (argc == 4) {
            params.game_type = parse_gametype(argv[3]);
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