#include <cstring>

#include "Players.hpp"

Players::Players()
  : _player_win_con()
  , _player_has_lost()
  , _player_previous_frame_dir()
  , _player_score()
  , _player_mvt_timer()
{}

void
Players::ResetPlayers(uint8_t nb_players)
{
    if (nb_players == 1) {
        _player_mvt_timer.time_ref[PLAYER_1] =
          std::chrono::high_resolution_clock::now();
        _player_mvt_timer.timer_values[PLAYER_1] = DEFAULT_SNAKE_TIMER_SECONDS;
    } else {
        _player_mvt_timer.time_ref[PLAYER_1] =
          std::chrono::high_resolution_clock::now();
        _player_mvt_timer.timer_values[PLAYER_1] = DEFAULT_SNAKE_TIMER_SECONDS;
        _player_mvt_timer.time_ref[PLAYER_2] =
          std::chrono::high_resolution_clock::now();
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
Players::DrawConclusion(uint8_t nb_player, IGraphic *gfx_interface)
{
    if (nb_player == 1) {
        _draw_game_end_single_player_ui(gfx_interface);
    } else {
        _draw_game_end_multi_player_ui(gfx_interface);
    }
}

void
Players::DrawPlayerStats(uint8_t nb_player, IGraphic *gfx_interface)
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