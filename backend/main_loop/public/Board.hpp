#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <random>

#include "Snake.hpp"
#include "WorldTypes.hpp"
#include "IGraphic.hpp"

class Board
{
  public:
    Board() = delete;
    Board(int32_t board_w, int32_t board_h);
    virtual ~Board() = default;
    Board(Board const &src) = delete;
    Board &operator=(Board const &rhs) = delete;
    Board(Board &&src) = delete;
    Board &operator=(Board &&rhs) = delete;

    [[nodiscard]] uint64_t currentUsedBoard() const;
    [[nodiscard]] uint8_t isFullNoFood() const;
    void checkPlayerState(
      uint8_t nb_player,
      std::array<WinCondition, NB_PLAYER_MAX> &player_win_con);
    void drawBoardElements(uint8_t nb_player, IGraphic *gfx_interface);
    void drawBoardStat(uint8_t nb_player, IGraphic *gfx_interface);
    void respawnFood();
    void handleBonusFood(double elapsed_time);
    void resetBoard(uint8_t generate_obstacles, uint8_t nb_player);
    std::array<Snake, NB_PLAYER_MAX> &updatePlayers();
    std::array<Snake, NB_PLAYER_MAX> const &getPlayers() const;
    Snake &updateFood();
    Snake &updateBonusFood();

  private:
    std::array<Snake, NB_PLAYER_MAX> _player;
    Snake _food;
    Snake _obstacle;
    Snake _bonus_food;

    int32_t _board_w;
    int32_t _board_h;
    uint64_t _board_size;

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

    void _generate_random_position(Snake &target,
                                   glm::vec3 const &color,
                                   uint64_t nb_to_add);
};

#endif
