#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include <array>
#include <chrono>

#include "IGraphic.hpp"
#include "WorldTypes.hpp"
#include "Snake.hpp"
#include "Board.hpp"
#include "math_utility.hpp"

class Players
{
  public:
    Players();
    virtual ~Players() = default;
    Players(Players const &src) = delete;
    Players &operator=(Players const &rhs) = delete;
    Players(Players &&src) = delete;
    Players &operator=(Players &&rhs) = delete;

    void resetPlayers(uint8_t nb_players);
    void drawConclusion(uint8_t nb_player, IGraphic *gfx_interface);
    void drawPlayerStats(uint8_t nb_player, IGraphic *gfx_interface);
    void moveSnakes(Board &board, uint8_t nb_players);
    [[nodiscard]] std::array<enum Snake::snakeDirection, NB_PLAYER_MAX> const &
    getSnakesPreviousDirection() const;
    std::array<WinCondition, NB_PLAYER_MAX> &updatePlayersWinConditionStates();
    std::array<uint8_t, NB_PLAYER_MAX> const &havePlayersLost();

  private:
    // Score related
    static constexpr uint64_t NORMAL_FOOD_VALUE = 100;
    static constexpr uint64_t BONUS_FOOD_VALUE = 500;

    // Timer related
    static double constexpr DEFAULT_SNAKE_TIMER_SECONDS =
      FRAME_LENGTH_SECONDS * 60;

    struct SnakeTimers
    {
        std::array<std::chrono::high_resolution_clock::time_point,
                   NB_PLAYER_MAX>
          time_ref;
        std::array<double, NB_PLAYER_MAX> timer_values;
    };

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

    void _draw_game_end_single_player_ui(IGraphic *gfx_interface);
    void _draw_game_end_multi_player_ui(IGraphic *gfx_interface);

    static uint8_t _will_snake_eat_food(Snake const &snake,
                                        Snake &food,
                                        glm::ivec2 &food_eaten_pos);
};

#endif
