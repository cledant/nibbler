#ifndef WORLDTYPES_HPP
#define WORLDTYPES_HPP

#include <string>

#include <glm/glm.hpp>

enum Player
{
    PLAYER_1 = 0,
    PLAYER_2,
};

struct UiElement
{
    std::string text;
    glm::vec3 color;
    glm::vec2 pos;
    float scale;
};

struct WinCondition
{
    uint8_t out_of_map;
    uint8_t touch_player;
    uint8_t touch_self;
    uint8_t touch_obstacle;
};

// Gfx related
static uint16_t constexpr MAX_FPS = 60;
static double constexpr FRAME_LENGTH_SECONDS = 1 / static_cast<float>(MAX_FPS);

// Player related
static constexpr uint8_t NB_PLAYER_MAX = 2;

// Ui related
static float constexpr ALIGN_BASE = 15.0f;
static float constexpr ALIGN_FIRST_TAB = 30.0f;

#endif
