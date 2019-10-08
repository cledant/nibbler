#ifndef WORLD_HPP
#define WORLD_HPP

#include "WorldInit.hpp"
#include "IGraphics.hpp"

class World
{
  public:
    World(WorldParams &&params);
    ~World() override = default;
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

    std::array<std::string.NB_GFX_LIB> _path_gfx_lib;
    std::array<uint8_t, IGraphicConstants::NB_EVENT> _events;

    // TODO Use proper class + array to store P1 / P2 / Apples
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> snake_color;
    uint32_t snake_size;

    void _init();
    void _get_events();
    void _should_be_updated();
    void _get_time();
    void _load_gfx_interface();
    void _stop_gfx_interface();
    void _move_snake();
};

#endif
