#include "World.hpp"

// UI
void
World::_draw_stats_ui()
{
    static const float align_base = 15.0f;
    static const float align_first_tab = 30.0f;
    static const UiElement time = {
        "Time: ", glm::vec3(1.0f), glm::vec2(align_base, 40.0f), 0.5f
    };
    static const UiElement player_1 = {
        "Player 1:", glm::vec3(1.0f), glm::vec2(align_base, 70.0f), 0.5f
    };
    static const UiElement player_1_snake_size = {
        "Snake size: ", glm::vec3(1.0f), glm::vec2(align_first_tab, 95.0f), 0.5f
    };
    static const UiElement player_1_snake_score = {
        "Score: ", glm::vec3(1.0f), glm::vec2(align_first_tab, 120.0f), 0.5f
    };
    static const UiElement player_2 = {
        "Player 2:", glm::vec3(1.0f), glm::vec2(align_base, 150.0f), 0.5f
    };
    static const UiElement player_2_snake_size = { "Snake size: ",
                                                   glm::vec3(1.0f),
                                                   glm::vec2(align_first_tab,
                                                             175.0f),
                                                   0.5f };
    static const UiElement player_2_snake_score = {
        "Score: ", glm::vec3(1.0f), glm::vec2(align_first_tab, 200.0f), 0.5f
    };

    _gfx_interface->drawText(time.text + std::to_string(static_cast<uint64_t>(
                                           std::floor(_game_length))),
                             player_1.color,
                             time.pos,
                             time.scale);
    _gfx_interface->drawText(
      player_1.text, player_1.color, player_1.pos, player_1.scale);
    _gfx_interface->drawText(
      player_1_snake_size.text +
        std::to_string(_player[PLAYER_1].getSnakeCurrentSize()),
      player_1_snake_size.color,
      player_1_snake_size.pos,
      player_1_snake_size.scale);
    _gfx_interface->drawText(player_1_snake_score.text +
                               std::to_string(_player_score[PLAYER_1]),
                             player_1_snake_score.color,
                             player_1_snake_score.pos,
                             player_1_snake_score.scale);
    if (_nb_player > 1) {
        _gfx_interface->drawText(
          player_2.text, player_2.color, player_2.pos, player_2.scale);
        _gfx_interface->drawText(
          player_2_snake_size.text +
            std::to_string(_player[PLAYER_2].getSnakeCurrentSize()),
          player_2_snake_size.color,
          player_2_snake_size.pos,
          player_2_snake_size.scale);
        _gfx_interface->drawText(player_2_snake_score.text +
                                   std::to_string(_player_score[PLAYER_2]),
                                 player_2_snake_score.color,
                                 player_2_snake_score.pos,
                                 player_2_snake_score.scale);
    }
}

void
World::_draw_interruption_ui()
{
    static const float align_base = 15.0f;
    static const UiElement pause = { "PAUSE",
                                     glm::vec3(1.0f, 0.0f, 0.0f),
                                     glm::vec2(align_base, 300.0f),
                                     1.0f };

    if (!_paused && !_game_ended) {
        return;
    }

    if (_paused && !_game_ended) {
        _gfx_interface->drawText(
          pause.text, pause.color, pause.pos, pause.scale);
    }

    if (_nb_player == 1) {
        _draw_game_end_single_player_ui();
    } else {
        _draw_game_end_multi_player_ui();
    }
}

void
World::_draw_game_end_single_player_ui()
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

    if (_game_ended && !_player_has_lost[PLAYER_1]) {
        _gfx_interface->drawText(win.text, win.color, win.pos, win.scale);
    } else if (_game_ended) {
        _gfx_interface->drawText(
          gameover.text, gameover.color, gameover.pos, gameover.scale);
    }
}

void
World::_draw_game_end_multi_player_ui()
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

    if (_game_ended) {
        if (_player_has_lost[PLAYER_1] && !_player_has_lost[PLAYER_2]) {
            _gfx_interface->drawText(player_2_won.text,
                                     player_2_won.color,
                                     player_2_won.pos,
                                     player_2_won.scale);
        } else if (!_player_has_lost[PLAYER_1] && _player_has_lost[PLAYER_2]) {
            _gfx_interface->drawText(player_1_won.text,
                                     player_1_won.color,
                                     player_1_won.pos,
                                     player_1_won.scale);

        } else if ((_player_has_lost[PLAYER_1] && _player_has_lost[PLAYER_2]) ||
                   (!_player_has_lost[PLAYER_1] &&
                    !_player_has_lost[PLAYER_2])) {
            if (_player_score[PLAYER_1] > _player_score[PLAYER_2]) {
                _gfx_interface->drawText(player_1_won.text,
                                         player_1_won.color,
                                         player_1_won.pos,
                                         player_1_won.scale);
            } else if (_player_score[PLAYER_2] > _player_score[PLAYER_1]) {
                _gfx_interface->drawText(player_2_won.text,
                                         player_2_won.color,
                                         player_2_won.pos,
                                         player_2_won.scale);
            } else {
                _gfx_interface->drawText(
                  draw.text, draw.color, draw.pos, draw.scale);
            }
        }
    }
}