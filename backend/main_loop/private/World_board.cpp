#include <cstring>

#include "World.hpp"

// Generating board
void
World::_generate_random_position(Snake &target,
                                 glm::vec3 const &color,
                                 uint64_t nb_to_add)
{
    for (uint64_t i = 0; i < nb_to_add; ++i) {
        while (_current_used_board() < _board_size) {
            auto new_obstacle =
              glm::ivec2(_dist_board_w(_mt_64), _dist_board_h(_mt_64));

            if (!_player[PLAYER_1].isInsideSnake(new_obstacle) &&
                !_player[PLAYER_2].isInsideSnake(new_obstacle) &&
                !_obstacle.isInsideSnake(new_obstacle) &&
                !_food.isInsideSnake(new_obstacle) &&
                !_bonus_food.isInsideSnake(new_obstacle)) {
                target.addToSnake(new_obstacle, color);
                break;
            }
        }
    }
}

void
World::_reset_game()
{
    _event_timers.timer_values = { SYSTEM_TIMER_SECONDS,
                                   PLAYER_TIMER_SECONDS,
                                   PLAYER_TIMER_SECONDS };
    _event_timers.time_ref = { std::chrono::high_resolution_clock::now(),
                               std::chrono::high_resolution_clock::now(),
                               std::chrono::high_resolution_clock::now() };
    std::memset(&_events, 0, sizeof(uint8_t) * IGraphicConstants::NB_EVENT);
    _reset_players();
    _reset_win_con();
    _reset_board_critters();
    _paused = 0;
    _game_ended = 0;
    _game_length = 0.0f;
}

void
World::_reset_players()
{
    if (_params.game_type == ONE_PLAYER) {
        _player[PLAYER_1].init(
          glm::uvec2{ _params.board_w / 2, _params.board_h / 2 },
          glm::vec3{ 0.0f, 1.0f, 0.0f },
          _params.board_w,
          _params.board_h);
        _player_mvt_timer.time_ref[PLAYER_1] =
          std::chrono::high_resolution_clock::now();
        _player_mvt_timer.timer_values[PLAYER_1] = DEFAULT_SNAKE_TIMER_SECONDS;
        _player[PLAYER_2].init(
          glm::vec3(1.0f), _params.board_w, _params.board_h);
    } else {
        _player[PLAYER_1].init(
          glm::uvec2{ _params.board_w / 4, _params.board_h / 2 },
          glm::vec3{ 0.0f, 1.0f, 0.0f },
          _params.board_w,
          _params.board_h);
        _player_mvt_timer.time_ref[PLAYER_1] =
          std::chrono::high_resolution_clock::now();
        _player_mvt_timer.timer_values[PLAYER_1] = DEFAULT_SNAKE_TIMER_SECONDS;
        _player[PLAYER_2].init(
          glm::uvec2{ 3 * _params.board_w / 4, _params.board_h / 2 },
          glm::vec3{ 0.0f, 0.0f, 1.0f },
          _params.board_w,
          _params.board_h);
        _player_mvt_timer.time_ref[PLAYER_2] =
          std::chrono::high_resolution_clock::now();
        _player_mvt_timer.timer_values[PLAYER_2] = DEFAULT_SNAKE_TIMER_SECONDS;
    }
}

void
World::_reset_win_con()
{
    std::memset(&_player_win_con, 0, sizeof(WinCondition) * NB_PLAYER_MAX);

    std::memset(&_player_previous_frame_dir,
                Snake::UP,
                sizeof(enum Snake::snakeDirection) * NB_PLAYER_MAX);
    std::memset(&_player_score, 0, sizeof(uint64_t) * NB_PLAYER_MAX);
    std::memset(&_player_has_lost, 0, sizeof(uint8_t) * NB_PLAYER_MAX);
}

void
World::_reset_board_critters()
{
    _obstacle.init(
      glm::vec3{ 0.33f, 0.33f, 0.33f }, _params.board_w, _params.board_h);
    _food.init(glm::vec3{ 1.0f, 0.0f, 0.0f }, _params.board_w, _params.board_h);
    _bonus_food.init(
      glm::vec3{ 0.5f, 0.0f, 0.0f }, _params.board_w, _params.board_h);
    _bonus_food_active = 0;
    if (_params.obstacles) {
        _generate_random_position(
          _obstacle, glm::vec3(0.33f), _dist_obstacle(_mt_64));
    }
    _bonus_food_active = 0;
    _bonus_spawn_chance = MAX_BONUS_SPAWN_CHANCE;
    _current_bonus_food_timer = 0.0;
}