#ifndef WORLD_HPP
#define WORLD_HPP

#include <array>

#include "WorldParams.hpp"
#include "IGraphic.hpp"

class World
{
  public:
    World(WorldParams &&params);
    virtual ~World() = default;
    World(World const &src) = delete;
    World &operator=(World const &rhs) = delete;
    World(World &&src) = delete;
    World &operator=(World &&rhs) = delete;

    void run();

  private:
    static double constexpr KEYBOARD_TIMER = 0.5;
    static uint32_t constexpr NB_GFX_LIB = 3;

    WorldParams _params;
    IGraphic *_gfx_interface;

    std::array<std::string, NB_GFX_LIB> _path_gfx_lib;
    std::array<uint8_t, IGraphicConstants::NB_EVENT> _events;

    // TODO Use proper class + array to store P1 / P2 / Apples
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> _snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> _snake_color;
    uint32_t _snake_size;

    void _init();
};

#endif
