#ifndef WORLDPARAMS_HPP
#define WORLDPARAMS_HPP

#include <string>

#include <cstdint>

enum GfxLib
{
    GFX_GLFW = 0,
    GFX_SFML,
    GFX_SDL,
    GFX_NONE,
};

enum SoundLib
{
    SOUND_SFML = 0,
    SOUND_NONE,
};

enum Gametype
{
    ONE_PLAYER = 0,
    TWO_PLAYER,
};

struct WorldParams
{
    int32_t board_w;
    int32_t board_h;
    enum GfxLib gfx_lib;
    enum SoundLib sound_lib;
    enum Gametype game_type;
    std::string home;
};

#endif
