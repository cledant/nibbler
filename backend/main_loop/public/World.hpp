#ifndef WORLD_HPP
#define WORLD_HPP

#include <array>
#include <chrono>
#include <random>

#include "WorldParams.hpp"
#include "IGraphic.hpp"
#include "dyn_lib_loader.hpp"
#include "Snake.hpp"
#include "math_utility.hpp"

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
    static double constexpr PLAYER_TIMER_SECONDS = FRAME_LENGTH_SECONDS;
    static double constexpr DEFAULT_SNAKE_TIMER_SECONDS =
      FRAME_LENGTH_SECONDS * 60;
    static uint8_t constexpr NB_EVENT_TIMER_TYPES = 3;
    static constexpr double BONUS_DURATION = 15.0;

    // Player related
    static constexpr uint8_t NB_PLAYER_MAX = 2;
    static constexpr uint64_t NORMAL_FOOD_VALUE = 100;
    static constexpr uint64_t BONUS_FOOD_VALUE = 500;

    // Bonus related
    static constexpr uint64_t MAX_BONUS_SPAWN_CHANCE = 3600;
    static constexpr uint64_t MAX_BONUS_STD_DEV = 300;

    enum EventTimersTypes
    {
        SYSTEM = 0,
        P1,
        P2,
    };

    enum Player
    {
        PLAYER_1 = 0,
        PLAYER_2,
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

    struct SnakeTimers
    {
        std::array<std::chrono::high_resolution_clock::time_point,
                   NB_PLAYER_MAX>
          time_ref;
        std::array<double, NB_PLAYER_MAX> timer_values;
    };

    struct WinCondition
    {
        uint8_t out_of_map;
        uint8_t touch_player;
        uint8_t touch_self;
        uint8_t touch_obstacle;
    };

    struct UiElement
    {
        std::string text;
        glm::vec3 color;
        glm::vec2 pos;
        float scale;
    };

    // Game related variables
    WorldParams _params;
    uint64_t _board_size;
    uint8_t _paused;
    uint8_t _nb_player;
    uint8_t _is_map_full;
    uint8_t _game_ended;
    double _game_length;

    // System related variables
    std::string _home;
    std::array<std::string, NB_GFX_LIB> _path_gfx_lib;
    DynLibLoader<IGraphic> _gfx_loader;
    IGraphic *_gfx_interface;
    uint8_t _is_init;
    std::chrono::high_resolution_clock ::time_point _loop_time_ref;

    // Event related variables
    std::array<uint8_t, IGraphicConstants::NB_EVENT> _events;
    EventTimers _event_timers;

    // Player related variables
    std::array<Snake, NB_PLAYER_MAX> _player;
    std::array<WinCondition, NB_PLAYER_MAX> _player_win_con;
    std::array<uint8_t, NB_PLAYER_MAX> _player_has_lost;
    std::array<enum Snake::snakeDirection, NB_PLAYER_MAX>
      _player_previous_frame_dir;
    std::array<uint64_t, NB_PLAYER_MAX> _player_score;
    SnakeTimers _player_mvt_timer;
    static constexpr std::array<double, IGraphicConstants::MAX_SNAKE_SIZE>
      _player_timers = [] {
          std::array<double, IGraphicConstants::MAX_SNAKE_SIZE> values{};

          // Generate timers value following this equation:
          // y = 0.16666 + 0.83333 * e^(-0.06480253 * x)

          for (uint64_t x = 0; x < IGraphicConstants::MAX_SNAKE_SIZE; ++x) {
              values[x] = 0.16666 + 0.83333 * exponential(-0.06480253 * x);
          }

          return (values);
      }();

    // Board critter related variables
    Snake _food;
    Snake _obstacle;
    Snake _bonus_food;

    // Bonus food related variables
    uint64_t _bonus_spawn_chance;
    uint8_t _bonus_food_active;
    double _current_bonus_food_timer;

    // Random generation related variables
    std::random_device _rd;
    std::mt19937_64 _mt_64;
    std::uniform_int_distribution<uint64_t> _dist_board_w;
    std::uniform_int_distribution<uint64_t> _dist_board_h;
    std::uniform_int_distribution<uint64_t> _dist_obstacle;
    std::normal_distribution<> _dist_chance_bonus;
    std::uniform_int_distribution<uint64_t> _dist_nb_bonus;

    // Dynamic lib related
    void _load_dyn_lib();
    void _clear_dyn_lib();

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

    // Moving snake
    void _move_snakes();
    uint8_t _will_snake_eat_food(Snake const &snake,
                                 glm::ivec2 &food_eaten_pos);
    uint8_t _will_snake_eat_bonus_food(Snake const &snake,
                                       glm::ivec2 &food_eaten_pos);

    // Win conditions handling
    uint64_t _current_used_board();
    void _check_player_state();
    void _should_game_end();

    // Generating board
    void _generate_random_position(Snake &target,
                                   glm::vec3 const &color,
                                   uint64_t nb_to_add);
    void _reset_game();
    void _reset_players();
    void _reset_win_con();
    void _reset_board_critters();

    // UI
    void _draw_stats_ui();
    void _draw_interruption_ui();
    void _draw_game_end_single_player_ui();
    void _draw_game_end_multi_player_ui();

    // Bonus food related
    void _respawn_food();
    void _handle_bonus_food(double elapsed_time);
};

#endif
