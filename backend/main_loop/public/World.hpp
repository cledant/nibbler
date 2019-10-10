#ifndef WORLD_HPP
#define WORLD_HPP

#include <array>
#include <chrono>

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
    static double constexpr KEYBOARD_TIMER_SECONDS = 0.5;
    static uint32_t constexpr NB_GFX_LIB = 3;
    static uint16_t constexpr MAX_FPS = 60;
    static double constexpr FRAME_LENGTH_SECONDS =
      1 / static_cast<float>(MAX_FPS);

    WorldParams _params;

    std::string _home;
    GfxLoader _gfx_loader;
    IGraphic *_gfx_interface;

    std::array<std::string, NB_GFX_LIB> _path_gfx_lib;
    std::array<uint8_t, IGraphicConstants::NB_EVENT> _events;

    // TODO Use proper class + array to store P1 / P2 / Apples
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> _snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> _snake_color;
    uint32_t _snake_size;

    uint8_t _is_init;

    std::chrono::high_resolution_clock ::time_point _time_ref;

    void _move_snakes();
    void _load_dyn_lib();
    void _clear_dyn_lib();
    void _get_events();
};

#endif
