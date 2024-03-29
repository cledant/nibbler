#include "Board.hpp"

Board::Board(int32_t board_w, int32_t board_h)
  : _player()
  , _board_w(board_w)
  , _board_h(board_h)
  , _board_size(board_w * board_h)
  , _bonus_spawn_chance(MAX_BONUS_SPAWN_CHANCE)
  , _bonus_food_active(0)
  , _current_bonus_food_timer(0.0)
  , _display_bonus_food(1)
  , _blink_frame_counter(BLINK_FRAME_MAX)
  , _rd()
  , _mt_64(_rd())
  , _dist_board_w(0, board_w - 1)
  , _dist_board_h(0, board_h - 1)
  , _dist_obstacle(0, (board_w * board_h) / 20)
  , _dist_chance_bonus(MAX_BONUS_SPAWN_CHANCE / 2.0, MAX_BONUS_STD_DEV)
  , _dist_nb_bonus(1, (board_w * board_h) / 50)
{}

uint64_t
Board::currentUsedBoard() const
{
    return (_food.getSnakeCurrentSize() + _bonus_food.getSnakeCurrentSize() +
            _obstacle.getSnakeCurrentSize() +
            _player[PLAYER_1].getSnakeCurrentSize() +
            _player[PLAYER_2].getSnakeCurrentSize());
}

uint8_t
Board::isFullNoFood() const
{
    if ((_obstacle.getSnakeCurrentSize() +
         _player[PLAYER_1].getSnakeCurrentSize() +
         _player[PLAYER_2].getSnakeCurrentSize() - _food.getSnakeCurrentSize() -
         _bonus_food.getSnakeCurrentSize()) < _board_size) {
        return (0);
    }
    return (1);
}

void
Board::checkPlayerState(uint8_t nb_player,
                        std::array<WinCondition, NB_PLAYER_MAX> &player_win_con)
{
#ifdef NDEBUG
    for (uint8_t i = 0; i < nb_player; ++i) {
        auto const &head_pos = _player[i].getSnakeHeadPos();

        // Check if player is inside board
        if (head_pos.x < 0 || head_pos.y < 0 || head_pos.x >= _board_w ||
            head_pos.y >= _board_h) {
            player_win_con[i].out_of_map = 1;
            continue;
        }

        // Check if players touch obstacles
        if (_obstacle.isInsideSnake(head_pos)) {
            player_win_con[i].touch_obstacle = 1;
            continue;
        }

        // Check if players touch itself
        if (_player[i].isSelfTouching()) {
            player_win_con[i].touch_self = 1;
            continue;
        }
    }
    if (nb_player > 1) {
        if (_player[PLAYER_1].isInsideSnake(
              _player[PLAYER_2].getSnakeHeadPos())) {
            player_win_con[PLAYER_2].touch_player = 1;
        }
        if (_player[PLAYER_2].isInsideSnake(
              _player[PLAYER_1].getSnakeHeadPos())) {
            player_win_con[PLAYER_1].touch_player = 1;
        }
    }
#else
    static_cast<void>(nb_player);
    static_cast<void>(player_win_con);
#endif
}

void
Board::drawBoardElements(uint8_t nb_player, IGraphic *gfx_interface)
{
    for (uint8_t i = 0; i < nb_player; ++i) {
        gfx_interface->drawSnake(_player[i].getSnakePosArray(),
                                 _player[i].getSnakeColorArray(),
                                 _player[i].getSnakeCurrentSize());
    }
    gfx_interface->drawSnake(_obstacle.getSnakePosArray(),
                             _obstacle.getSnakeColorArray(),
                             _obstacle.getSnakeCurrentSize());
    gfx_interface->drawSnake(_food.getSnakePosArray(),
                             _food.getSnakeColorArray(),
                             _food.getSnakeCurrentSize());
    if (_display_bonus_food) {
        gfx_interface->drawSnake(_bonus_food.getSnakePosArray(),
                                 _bonus_food.getSnakeColorArray(),
                                 _bonus_food.getSnakeCurrentSize());
    }
}

void
Board::drawBoardStat(uint8_t nb_player, IGraphic *gfx_interface)
{
    static const UiElement player_1_snake_size = {
        "Snake size: ", glm::vec3(1.0f), glm::vec2(ALIGN_FIRST_TAB, 95.0f), 0.5f
    };
    static const UiElement player_2_snake_size = { "Snake size: ",
                                                   glm::vec3(1.0f),
                                                   glm::vec2(ALIGN_FIRST_TAB,
                                                             175.0f),
                                                   0.5f };

    gfx_interface->drawText(
      player_1_snake_size.text +
        std::to_string(_player[PLAYER_1].getSnakeCurrentSize()),
      player_1_snake_size.color,
      player_1_snake_size.pos,
      player_1_snake_size.scale);
    if (nb_player > 1) {
        gfx_interface->drawText(
          player_2_snake_size.text +
            std::to_string(_player[PLAYER_2].getSnakeCurrentSize()),
          player_2_snake_size.color,
          player_2_snake_size.pos,
          player_2_snake_size.scale);
    }
}

void
Board::respawnFood()
{
    if (!_food.getSnakeCurrentSize() && currentUsedBoard() < _board_size) {
        _generate_random_position(_food, glm::vec3(1.0f, 0.0f, 0.0f), 1);
    }
}

void
Board::handleBonusFood(double elapsed_time, IAudio *audio_interface)
{
    if (!_bonus_food_active) {
        if (_dist_chance_bonus(_mt_64) > _bonus_spawn_chance) {
            _bonus_food_active = 1;
            _current_bonus_food_timer = BONUS_DURATION;
            _display_bonus_food = 1;
            _blink_frame_counter = BLINK_FRAME_MAX;
            _generate_random_position(
              _bonus_food, glm::vec3(0.5f, 0.0f, 0.0f), _dist_nb_bonus(_mt_64));
            if (audio_interface) {
                audio_interface->playSound(IAudioTypes::BONUS);
            }
        } else if (_bonus_spawn_chance) {
            --_bonus_spawn_chance;
        } else {
            _bonus_spawn_chance = MAX_BONUS_SPAWN_CHANCE;
        }
    } else {
        _current_bonus_food_timer -= elapsed_time;
        if (_current_bonus_food_timer < BLINK_START) {
            --_blink_frame_counter;
            if (!_blink_frame_counter) {
                _display_bonus_food = !_display_bonus_food;
                _blink_frame_counter = BLINK_FRAME_MAX;
            }
        }
        if (!_bonus_food.getSnakeCurrentSize() ||
            _current_bonus_food_timer <= 0.0) {
            _bonus_food_active = 0;
            _bonus_spawn_chance = MAX_BONUS_SPAWN_CHANCE;
            _bonus_food.init(glm::vec3{ 0.5f, 0.0f, 0.0f }, _board_w, _board_h);
        }
    }
}

void
Board::resetBoard(uint8_t generate_obstacles, uint8_t nb_player)
{
    if (nb_player == 1) {
        _player[PLAYER_1].init(glm::uvec2{ _board_w / 2, _board_h / 2 },
                               glm::vec3{ 0.0f, 1.0f, 0.0f },
                               _board_w,
                               _board_h);
        _player[PLAYER_2].init(glm::vec3(1.0f), _board_w, _board_h);
    } else {
        _player[PLAYER_1].init(glm::uvec2{ _board_w / 4, _board_h / 2 },
                               glm::vec3{ 0.0f, 1.0f, 0.0f },
                               _board_w,
                               _board_h);
        _player[PLAYER_2].init(glm::uvec2{ 3 * _board_w / 4, _board_h / 2 },
                               glm::vec3{ 0.0f, 0.0f, 1.0f },
                               _board_w,
                               _board_h);
    }
    _obstacle.init(glm::vec3{ 0.33f, 0.33f, 0.33f }, _board_w, _board_h);
    _food.init(glm::vec3{ 1.0f, 0.0f, 0.0f }, _board_w, _board_h);
    _bonus_food.init(glm::vec3{ 0.5f, 0.0f, 0.0f }, _board_w, _board_h);
    _bonus_food_active = 0;
    if (generate_obstacles) {
        //     _generate_obstacles(glm::vec3(0.33f), _dist_obstacle(_mt_64));
        _generate_obstacles(glm::vec3(0.33f), 5);
    }
    _bonus_food_active = 0;
    _bonus_spawn_chance = MAX_BONUS_SPAWN_CHANCE;
    _current_bonus_food_timer = 0.0;
    _display_bonus_food = 1;
    _blink_frame_counter = BLINK_FRAME_MAX;
}

std::array<Snake, NB_PLAYER_MAX> &
Board::updatePlayers()
{
    return (_player);
}

std::array<Snake, NB_PLAYER_MAX> const &
Board::getPlayers() const
{
    return (_player);
}

Snake &
Board::updateFood()
{
    return (_food);
}

Snake &
Board::updateBonusFood()
{
    return (_bonus_food);
}

void
Board::_generate_random_position(Snake &target,
                                 glm::vec3 const &color,
                                 uint64_t nb_to_add)
{
    for (uint64_t i = 0; i < nb_to_add; ++i) {
        while (currentUsedBoard() < _board_size) {
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
Board::_generate_obstacles(glm::vec3 const &color, uint64_t nb_to_add)
{
    for (uint64_t i = 0; i < nb_to_add; ++i) {
        uint64_t watchdog = DEFAULT_WATCHDOG;

        while (currentUsedBoard() < _board_size) {
            auto new_obstacle =
              glm::ivec2(_dist_board_w(_mt_64), _dist_board_h(_mt_64));

            --watchdog;
            if (!watchdog) {
                return;
            }

            if (!_valid_obstacle(new_obstacle)) {
                continue;
            }
            if (!_player[PLAYER_1].isInsideSnake(new_obstacle) &&
                !_player[PLAYER_2].isInsideSnake(new_obstacle) &&
                !_food.isInsideSnake(new_obstacle) &&
                !_bonus_food.isInsideSnake(new_obstacle) &&
                !_obstacle.isInsideSnake(new_obstacle)) {
                _obstacle.addToSnake(new_obstacle, color);
                break;
            }
        }
    }
}

uint8_t
Board::_valid_obstacle(glm::ivec2 const &pos)
{
    std::array<glm::ivec2, 12> const forbidden_corners = {
        glm::ivec2{ 0, 1 },
        glm::ivec2{ 1, 0 },
        glm::ivec2{ _board_w - 2, 0 },
        glm::ivec2{ _board_w - 1, 1 },
        glm::ivec2{ 0, _board_h - 2 },
        glm::ivec2{ 1, _board_h - 1 },
        glm::ivec2{ _board_w - 2, _board_h - 1 },
        glm::ivec2{ _board_w - 1, _board_h - 2 },
        glm::ivec2{ 0, 0 },
        glm::ivec2{ _board_w - 1, _board_h - 1 },
        glm::ivec2{ 0, _board_h - 1 },
        glm::ivec2{ _board_w - 1, 0 }
    };

    for (auto const &it : forbidden_corners) {
        if (it == pos) {
            return (0);
        }
    }

    std::array<glm::ivec2, 8> const forbidden_pos = {
        glm::ivec2(pos.x + 2, pos.y),     glm::ivec2(pos.x, pos.y + 2),
        glm::ivec2(pos.x - 2, pos.y),     glm::ivec2(pos.x, pos.y - 2),
        glm::ivec2(pos.x + 1, pos.y + 1), glm::ivec2(pos.x + 1, pos.y - 1),
        glm::ivec2(pos.x - 1, pos.y + 1), glm::ivec2(pos.x - 1, pos.y - 1)
    };

    for (auto const &it : forbidden_pos) {
        if (it.x < 0 || it.x >= _board_w || it.y < 0 || it.y >= _board_h) {
            continue;
        }
        if (_obstacle.isInsideSnake(it)) {
            return (0);
        }
    }
    return (1);
}
