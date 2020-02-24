#include <iostream>
#include <cstring>

#include "World.hpp"

World::World(WorldParams const &params)
  : _params(params)
  , _paused(0)
  , _nb_player(_params.game_type + 1)
  , _is_map_full(0)
  , _game_ended(0)
  , _game_length(0.0f)
  , _home()
  , _path_gfx_lib()
  , _gfx_loader()
  , _gfx_interface(nullptr)
  , _is_init(0)
  , _loop_time_ref(std::chrono::high_resolution_clock::now())
  , _events()
  , _event_timers()
  , _players()
  , _board(params.board_h, params.board_w)
{
    _reset_game();
}

World::~World()
{
    _clear_dyn_lib();
}

void
World::init()
{
    if (!_is_init) {
        _home = getenv("HOME");
        if (_home.empty()) {
            throw std::runtime_error("Home not set");
        }
#ifdef __APPLE__
        _path_gfx_lib[GFX_GLFW] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.dylib";
        _path_gfx_lib[GFX_SFML] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_sfml.dylib";
        _path_gfx_lib[GFX_SDL] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_sdl2.dylib";
#else
        _path_gfx_lib[GFX_GLFW] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
        _path_gfx_lib[GFX_SFML] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_sfml.so";
        _path_gfx_lib[GFX_SDL] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_sdl2.so";
#endif
        _load_dyn_lib();
        _is_init = 1;
    }
}

void
World::run()
{
    while (!_gfx_interface->shouldClose()) {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> loop_diff = now - _loop_time_ref;

        if (loop_diff.count() > FRAME_LENGTH_SECONDS) {
            _gfx_interface->getEvents(_events);
            _interpret_events();
            if (!_game_ended && !_paused) {
                _move_snakes();
                _check_player_state();
                _should_game_end();
                _board.RespawnFood();
                _board.HandleBonusFood(loop_diff.count());
                _game_length += loop_diff.count();
            }
            if (_gfx_interface) {
                _gfx_interface->clear();
                _gfx_interface->drawBoard();
                _board.DrawBoardElements(_nb_player, _gfx_interface);
                _board.DrawBoardStat(_nb_player, _gfx_interface);
                _players.DrawPlayerStats(_nb_player, _gfx_interface);
                _draw_game_stats_ui();
                _draw_interruption_ui();
                _gfx_interface->render();
            }
            _loop_time_ref = now;
        }
    }
}

// Dynamic lib related
void
World::_load_dyn_lib()
{
    if (!_gfx_interface) {
        _gfx_loader.openLib(_path_gfx_lib[_params.gfx_lib]);
        _gfx_interface = _gfx_loader.getCreator()();
        _gfx_interface->init(_home, _params.board_w, _params.board_h);
        _gfx_interface->createWindow("Nibbler");
    }
}

void
World::_clear_dyn_lib()
{
    if (_gfx_interface) {
        _gfx_interface->deleteWindow();
        _gfx_interface->terminate();
        _gfx_loader.getDeleter()(_gfx_interface);
        _gfx_loader.closeLib();
        _gfx_interface = nullptr;
    }
}

// Moving snake
void
World::_move_snakes()
{
    auto now = std::chrono::high_resolution_clock::now();

    for (uint8_t i = 0; i < _nb_player; ++i) {
        std::chrono::duration<double> time_diff =
          now - _player_mvt_timer.time_ref[i];

        if (time_diff.count() > _player_mvt_timer.timer_values[i]) {
            glm::ivec2 food_eaten_pos(-1);

            if (_will_snake_eat_food(_player[i], food_eaten_pos)) {
                _player[i].addToSnake(food_eaten_pos);
                _player_score[i] += NORMAL_FOOD_VALUE;
                _player_mvt_timer.timer_values[i] =
                  _player_timers[_player[i].getSnakeCurrentSize()];
            } else if (_will_snake_eat_bonus_food(_player[i], food_eaten_pos)) {
                _player[i].addToSnake(food_eaten_pos);
                _player_score[i] += BONUS_FOOD_VALUE;
                _player_mvt_timer.timer_values[i] =
                  _player_timers[_player[i].getSnakeCurrentSize()];
            } else {
                _player[i].moveSnakeWithCurrentDirection();
            }
            _player_previous_frame_dir[i] = _player[i].getSnakeDirection();
            _player_mvt_timer.time_ref[i] = now;
        }
    }
}

uint8_t
World::_will_snake_eat_food(Snake const &snake, glm::ivec2 &food_eaten_pos)
{
    auto next_head_pos = snake.getInFrontOfSnakeHeadPos();
    auto food_array = _food.getSnakePosArray();
    auto food_size = _food.getSnakeCurrentSize();

    for (uint64_t i = 0; i < food_size; ++i) {
        if (food_array[i] == next_head_pos) {
            _food.removeFromSnake(next_head_pos);
            food_eaten_pos = next_head_pos;
            return (1);
        }
    }
    return (0);
}

uint8_t
World::_will_snake_eat_bonus_food(Snake const &snake,
                                  glm::ivec2 &food_eaten_pos)
{
    auto next_head_pos = snake.getInFrontOfSnakeHeadPos();
    auto food_array = _bonus_food.getSnakePosArray();
    auto food_size = _bonus_food.getSnakeCurrentSize();

    for (uint64_t i = 0; i < food_size; ++i) {
        if (food_array[i] == next_head_pos) {
            _bonus_food.removeFromSnake(next_head_pos);
            food_eaten_pos = next_head_pos;
            return (1);
        }
    }
    return (0);
}

void
World::_should_game_end()
{
    static const struct WinCondition not_lost = { 0, 0, 0, 0 };

    if (_game_ended) {
        return;
    }

    _player_has_lost[PLAYER_1] = memcmp(
      &_player_win_con[PLAYER_1], &not_lost, sizeof(struct WinCondition));
    _player_has_lost[PLAYER_2] = memcmp(
      &_player_win_con[PLAYER_2], &not_lost, sizeof(struct WinCondition));

    if (_player_has_lost[PLAYER_1] || _player_has_lost[PLAYER_2]) {
        _game_ended = 1;
    } else if ((_current_used_board() - _food.getSnakeCurrentSize() -
                _bonus_food.getSnakeCurrentSize()) == _board_size) {
        _game_ended = 1;
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
    _players.ResetPlayers(_nb_player);
    _board.ResetBoard(_params.obstacles, _nb_player);
    _paused = 0;
    _game_ended = 0;
    _game_length = 0.0f;
}

void
World::_draw_game_stats_ui()
{
    static const UiElement time = {
        "Time: ", glm::vec3(1.0f), glm::vec2(ALIGN_BASE, 40.0f), 0.5f
    };
    static const UiElement player_1 = {
        "Player 1:", glm::vec3(1.0f), glm::vec2(ALIGN_BASE, 70.0f), 0.5f
    };
    static const UiElement player_2 = {
        "Player 2:", glm::vec3(1.0f), glm::vec2(ALIGN_BASE, 150.0f), 0.5f
    };

    _gfx_interface->drawText(time.text + std::to_string(static_cast<uint64_t>(
                                           std::floor(_game_length))),
                             player_1.color,
                             time.pos,
                             time.scale);
    _gfx_interface->drawText(
      player_1.text, player_1.color, player_1.pos, player_1.scale);
    if (_nb_player > 1) {
        _gfx_interface->drawText(
          player_2.text, player_2.color, player_2.pos, player_2.scale);
    }
}

void
World::_draw_interruption_ui()
{
    static const UiElement pause = { "PAUSE",
                                     glm::vec3(1.0f, 0.0f, 0.0f),
                                     glm::vec2(ALIGN_BASE, 300.0f),
                                     1.0f };

    if (!_paused && !_game_ended) {
        return;
    }
    if (_paused && !_game_ended) {
        _gfx_interface->drawText(
          pause.text, pause.color, pause.pos, pause.scale);
    }
    if (_game_ended) {
        _players.DrawConclusion(_nb_player, _gfx_interface);
    }
}