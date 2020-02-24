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
static uint32_t constexpr NB_GFX_LIB = 3;
static uint16_t constexpr MAX_FPS = 60;
static double constexpr FRAME_LENGTH_SECONDS = 1 / static_cast<float>(MAX_FPS);

// Bonus related
static constexpr uint64_t MAX_BONUS_SPAWN_CHANCE = 3600;
static constexpr uint64_t MAX_BONUS_STD_DEV = 300;

// Player related
static constexpr uint8_t NB_PLAYER_MAX = 2;
static constexpr uint64_t NORMAL_FOOD_VALUE = 100;
static constexpr uint64_t BONUS_FOOD_VALUE = 500;

// Timer related
static double constexpr SYSTEM_TIMER_SECONDS = 1.0;
static double constexpr PLAYER_TIMER_SECONDS = FRAME_LENGTH_SECONDS;
static double constexpr DEFAULT_SNAKE_TIMER_SECONDS = FRAME_LENGTH_SECONDS * 60;
static uint8_t constexpr NB_EVENT_TIMER_TYPES = 3;
static constexpr double BONUS_DURATION = 15.0;

// Ui related
static float constexpr ALIGN_BASE = 15.0f;
static float constexpr ALIGN_FIRST_TAB = 30.0f;

#endif
