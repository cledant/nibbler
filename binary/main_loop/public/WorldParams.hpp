#ifndef WORLDPARAMS_HPP
#define WORLDPARAMS_HPP

enum GfxLib
{
    GLFW = 0,
    SFML,
    SDL,
    NONE,
};

enum SoundLib
{
    SFML = 0,
    NONE,
};

enum Gametype
{
    ONE_PLAYER = 0,
    TWO_PLAYER,
};

struct WorldParams
{
    int32_t board_w;
    int32_t board_w;
    enum GfxLib gfx_lib;
    enum Gametype game_type;
    enum SoundLib sound_lib;
    std::string _home;
};

#endif
