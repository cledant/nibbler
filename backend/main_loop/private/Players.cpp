#include <cstring>

#include "Players.hpp"

#include "WorldParams.hpp"

Players::Players()
  : _player_win_con()
  , _player_has_lost()
  , _player_previous_frame_dir()
  , _player_score()
  , _player_mvt_timer()
{}

void
Players::resetPlayers(uint8_t nb_players)
{
    if (nb_players == 1) {
        _player_mvt_timer.time_ref[PLAYER_1] = std::chrono::steady_clock::now();
        _player_mvt_timer.timer_values[PLAYER_1] = DEFAULT_SNAKE_TIMER_SECONDS;
    } else {
        _player_mvt_timer.time_ref[PLAYER_1] = std::chrono::steady_clock::now();
        _player_mvt_timer.timer_values[PLAYER_1] = DEFAULT_SNAKE_TIMER_SECONDS;
        _player_mvt_timer.time_ref[PLAYER_2] = std::chrono::steady_clock::now();
        _player_mvt_timer.timer_values[PLAYER_2] = DEFAULT_SNAKE_TIMER_SECONDS;
    }
    std::memset(&_player_win_con, 0, sizeof(WinCondition) * NB_PLAYER_MAX);

    std::memset(&_player_previous_frame_dir,
                Snake::UP,
                sizeof(enum Snake::snakeDirection) * NB_PLAYER_MAX);
    std::memset(&_player_score, 0, sizeof(uint64_t) * NB_PLAYER_MAX);
    std::memset(&_player_has_lost, 0, sizeof(uint8_t) * NB_PLAYER_MAX);
}

void
Players::drawConclusion(uint8_t nb_player, IGraphic *gfx_interface)
{
    if (nb_player == 1) {
        _draw_game_end_single_player_ui(gfx_interface);
    } else {
        _draw_game_end_multi_player_ui(gfx_interface);
    }
}

void
Players::drawPlayerStats(uint8_t nb_player, IGraphic *gfx_interface)
{
    static const UiElement player_1_snake_score = {
        "Score: ", glm::vec3(1.0f), glm::vec2(ALIGN_FIRST_TAB, 120.0f), 0.5f
    };
    static const UiElement player_2_snake_score = {
        "Score: ", glm::vec3(1.0f), glm::vec2(ALIGN_FIRST_TAB, 200.0f), 0.5f
    };

    gfx_interface->drawText(player_1_snake_score.text +
                              std::to_string(_player_score[PLAYER_1]),
                            player_1_snake_score.color,
                            player_1_snake_score.pos,
                            player_1_snake_score.scale);
    if (nb_player > 1) {
        gfx_interface->drawText(player_2_snake_score.text +
                                  std::to_string(_player_score[PLAYER_2]),
                                player_2_snake_score.color,
                                player_2_snake_score.pos,
                                player_2_snake_score.scale);
    }
}

void
Players::moveSnakes(Board &board, uint8_t nb_players, IAudio *audio_interface)
{
    auto now = std::chrono::steady_clock::now();
    auto &player = board.updatePlayers();
    auto &bonus_food = board.updateBonusFood();
    auto &food = board.updateFood();

    for (uint8_t i = 0; i < nb_players; ++i) {
        std::chrono::duration<double> time_diff =
          now - _player_mvt_timer.time_ref[i];

        if (time_diff.count() > _player_mvt_timer.timer_values[i]) {
            glm::ivec2 food_eaten_pos(-1);

            if (_will_snake_eat_food(player[i], food, food_eaten_pos)) {
                player[i].addToSnake(food_eaten_pos);
                _player_score[i] += NORMAL_FOOD_VALUE;
                _player_mvt_timer.timer_values[i] =
                  _player_timers[player[i].getSnakeCurrentSize()];
                if (audio_interface) {
                    audio_interface->playSound(IAudioTypes::EAT);
                }
            } else if (_will_snake_eat_food(
                         player[i], bonus_food, food_eaten_pos)) {
                player[i].addToSnake(food_eaten_pos);
                _player_score[i] += BONUS_FOOD_VALUE;
                _player_mvt_timer.timer_values[i] =
                  _player_timers[player[i].getSnakeCurrentSize()];
                if (audio_interface) {
                    audio_interface->playSound(IAudioTypes::EAT);
                }
            } else {
                player[i].moveSnakeWithCurrentDirection();
            }
            _player_previous_frame_dir[i] = player[i].getSnakeDirection();
            _player_mvt_timer.time_ref[i] = now;
        }
    }
}

std::array<enum Snake::snakeDirection, NB_PLAYER_MAX> const &
Players::getSnakesPreviousDirection() const
{
    return (_player_previous_frame_dir);
}

std::array<WinCondition, NB_PLAYER_MAX> &
Players::updatePlayersWinConditionStates()
{
    return (_player_win_con);
}

std::array<uint32_t, NB_PLAYER_MAX> const &
Players::havePlayersLost()
{
    _player_has_lost[PLAYER_1] = _player_win_con[PLAYER_1].out_of_map |
                                 (_player_win_con[PLAYER_1].touch_player) << 8 |
                                 (_player_win_con[PLAYER_1].touch_self) << 16 |
                                 (_player_win_con[PLAYER_1].touch_obstacle)
                                   << 24;
    _player_has_lost[PLAYER_2] = _player_win_con[PLAYER_2].out_of_map |
                                 (_player_win_con[PLAYER_2].touch_player) << 8 |
                                 (_player_win_con[PLAYER_2].touch_self) << 16 |
                                 (_player_win_con[PLAYER_2].touch_obstacle)
                                   << 24;
    return (_player_has_lost);
}

void
Players::_draw_game_end_single_player_ui(IGraphic *gfx_interface)
{
    static const float align_base = 15.0f;
    static const UiElement win = { "YOU WIN",
                                   glm::vec3(0.0f, 0.0f, 1.0f),
                                   glm::vec2(align_base, 350.0f),
                                   1.0f };
    static const UiElement gameover = { "GAMEOVER",
                                        glm::vec3(1.0f, 0.0f, 0.0f),
                                        glm::vec2(align_base, 350.0f),
                                        1.0f };

    if (!_player_has_lost[PLAYER_1]) {
        gfx_interface->drawText(win.text, win.color, win.pos, win.scale);
    } else {
        gfx_interface->drawText(
          gameover.text, gameover.color, gameover.pos, gameover.scale);
    }
}

void
Players::_draw_game_end_multi_player_ui(IGraphic *gfx_interface)
{
    static const float align_base = 15.0f;
    static const UiElement player_1_won = { "PLAYER 1 WON",
                                            glm::vec3(0.0f, 1.0f, 0.0f),
                                            glm::vec2(align_base, 350.0f),
                                            1.0f };
    static const UiElement player_2_won = { "PLAYER 2 WON",
                                            glm::vec3(0.0f, 0.0f, 1.0f),
                                            glm::vec2(align_base, 350.0f),
                                            1.0f };
    static const UiElement draw = {
        "DRAW", glm::vec3(1.0f), glm::vec2(align_base, 350.0f), 1.0f
    };

    if (_player_has_lost[PLAYER_1] && !_player_has_lost[PLAYER_2]) {
        gfx_interface->drawText(player_2_won.text,
                                player_2_won.color,
                                player_2_won.pos,
                                player_2_won.scale);
    } else if (!_player_has_lost[PLAYER_1] && _player_has_lost[PLAYER_2]) {
        gfx_interface->drawText(player_1_won.text,
                                player_1_won.color,
                                player_1_won.pos,
                                player_1_won.scale);

    } else if ((_player_has_lost[PLAYER_1] && _player_has_lost[PLAYER_2]) ||
               (!_player_has_lost[PLAYER_1] && !_player_has_lost[PLAYER_2])) {
        if (_player_score[PLAYER_1] > _player_score[PLAYER_2]) {
            gfx_interface->drawText(player_1_won.text,
                                    player_1_won.color,
                                    player_1_won.pos,
                                    player_1_won.scale);
        } else if (_player_score[PLAYER_2] > _player_score[PLAYER_1]) {
            gfx_interface->drawText(player_2_won.text,
                                    player_2_won.color,
                                    player_2_won.pos,
                                    player_2_won.scale);
        } else {
            gfx_interface->drawText(
              draw.text, draw.color, draw.pos, draw.scale);
        }
    }
}

uint8_t
Players::_will_snake_eat_food(Snake const &snake,
                              Snake &food,
                              glm::ivec2 &food_eaten_pos)
{
    auto const &next_head_pos = snake.getInFrontOfSnakeHeadPos();
    auto const &food_array = food.getSnakePosArray();
    auto const &food_size = food.getSnakeCurrentSize();

    for (uint64_t i = 0; i < food_size; ++i) {
        if (food_array[i] == next_head_pos) {
            food.removeFromSnake(next_head_pos);
            food_eaten_pos = next_head_pos;
            return (1);
        }
    }
    return (0);
}