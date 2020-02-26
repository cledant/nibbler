#ifndef WORLD_HPP
#define WORLD_HPP

#include <array>
#include <chrono>

#include "WorldParams.hpp"
#include "WorldTypes.hpp"
#include "IGraphic.hpp"
#include "IAudio.hpp"
#include "dyn_lib_loader.hpp"
#include "Snake.hpp"
#include "Board.hpp"
#include "Players.hpp"

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
    static uint32_t constexpr NB_GFX_LIB = 3;
    static uint32_t constexpr NB_AUDIO_LIB = 3;

    // Event related
    static uint8_t constexpr NB_EVENT_TIMER_TYPES = 3;
    static double constexpr SYSTEM_TIMER_SECONDS = 1.0;
    static double constexpr PLAYER_TIMER_SECONDS = FRAME_LENGTH_SECONDS;

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

    // Game related variables
    WorldParams _params;
    uint8_t _paused;
    uint8_t _nb_player;
    uint8_t _is_map_full;
    uint8_t _game_ended;
    double _game_length;

    // System related variables
    std::string _home;
    std::array<std::string, NB_GFX_LIB> _path_gfx_lib;
    std::array<std::string, NB_AUDIO_LIB> _path_audio_lib;
    DynLibLoader<IGraphic> _gfx_loader;
    DynLibLoader<IAudio> _audio_loader;
    IGraphic *_gfx_interface;
    IAudio *_audio_interface;
    uint8_t _is_init;
    std::chrono::high_resolution_clock ::time_point _loop_time_ref;

    // Event related variables
    std::array<uint8_t, IGraphicConstants::NB_EVENT> _events;
    EventTimers _event_timers;

    Players _players;
    Board _board;

    // Dynamic lib related
    void _load_gfx_dyn_lib();
    void _clear_gfx_dyn_lib();
    void _load_audio_dyn_lib();
    void _clear_audio_dyn_lib();

    // Event handling functions
    void _interpret_events();
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

    // Game related
    void _should_game_end(Players &players, Board const &board);
    void _reset_game();

    // UI
    void _draw_game_stats_ui();
    void _draw_interruption_ui();
};

#endif
