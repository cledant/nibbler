#ifndef WORLD_HPP
#define WORLD_HPP

#include <array>

#include "WorldParams.hpp"
#include "IGraphic.hpp"
#include "gfx_loader.hpp"

class World
{
  public:
    explicit World(WorldParams const &params);
    virtual ~World();
    World(World const &src) = delete;
    World &operator=(World const &rhs) = delete;
    World(World &&src) = delete;
    World &operator=(World &&rhs) = delete;

    void init();
    void run();

  private:
    static double constexpr KEYBOARD_TIMER = 0.5;
    static uint32_t constexpr NB_GFX_LIB = 3;

    WorldParams _params;
    GfxLoader _gfx_loader;
    IGraphic *_gfx_interface;

    std::string _home;
    std::array<std::string, NB_GFX_LIB> _path_gfx_lib;
    // TODO Update to std::array
    uint8_t _events[IGraphicConstants::NB_EVENT];

    // TODO Use proper class + array to store P1 / P2 / Apples
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> _snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> _snake_color;
    uint32_t _snake_size;

    uint8_t _is_init;

    void _load_dyn_lib();
    void _clear_dyn_lib();
    void _get_events();
};

#endif
