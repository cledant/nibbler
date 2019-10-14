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
    World() = delete;
    explicit World(WorldParams const &params);
    virtual ~World();
    World(World const &src) = delete;
    World &operator=(World const &rhs) = delete;
    World(World &&src) = delete;
    World &operator=(World &&rhs) = delete;

    void init();
    void run();

  private:
    // Gfx related
    static uint32_t constexpr NB_GFX_LIB = 3;
    static uint16_t constexpr MAX_FPS = 60;
    static double constexpr FRAME_LENGTH_SECONDS =
      1 / static_cast<float>(MAX_FPS);

    // Timer related
    static double constexpr SYSTEM_TIMER_SECONDS = 1.0;
    static double constexpr DEFAULT_SNAKE_TIMER_SECONDS =
      FRAME_LENGTH_SECONDS * 30;
    static uint8_t constexpr NB_EVENT_TIMER_TYPES = 3;

    enum EventTimersTypes
    {
        SYSTEM = 0,
        P1,
        P2,
    };

    struct EventTimers
    {
        std::array<uint8_t, NB_EVENT_TIMER_TYPES> accept_event;
        std::array<uint8_t, NB_EVENT_TIMER_TYPES> updated;
        std::array<std::chrono::high_resolution_clock::time_point,
                   NB_EVENT_TIMER_TYPES>
          time_ref;
        std::array<double, NB_EVENT_TIMER_TYPES> timer_values;
    };

    WorldParams _params;

    std::string _home;
    GfxLoader _gfx_loader;
    IGraphic *_gfx_interface;

    std::array<std::string, NB_GFX_LIB> _path_gfx_lib;
    std::array<uint8_t, IGraphicConstants::NB_EVENT> _events;
    EventTimers _event_timers;

    // TODO Use proper class + array to store P1 / P2 / Apples
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> _snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> _snake_color;
    uint32_t _snake_size;

    uint8_t _is_init;
    uint8_t _paused;

    std::chrono::high_resolution_clock ::time_point _loop_time_ref;

    void _load_dyn_lib();
    void _clear_dyn_lib();
    void _interpret_events();

    // Event handling functions
    void _close_win_event();
    void _pause();
    void _toggle_fullscreen();
    void _p1_up();
    void _p1_right();
    void _p1_down();
    void _p1_left();
    void _p2_up();
    void _p2_right();
    void _p2_down();
    void _p2_left();
    void _set_glfw();
    void _set_sfml();
    void _set_sdl();
};

#endif
