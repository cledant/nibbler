#include <iostream>
#include <cstring>

#include "World.hpp"

World::World(WorldParams const &params)
  : _params(params)
  , _board_size(params.board_h * params.board_w)
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
  , _player()
  , _player_win_con()
  , _player_has_lost()
  , _player_previous_frame_dir()
  , _player_score()
  , _player_mvt_timer()
  , _bonus_spawn_chance(MAX_BONUS_SPAWN_CHANCE)
  , _bonus_food_active(0)
  , _current_bonus_food_timer(0.0)
  , _rd()
  , _mt_64(_rd())
  , _dist_board_w(0, _params.board_w - 1)
  , _dist_board_h(0, _params.board_h - 1)
  , _dist_obstacle(0, _board_size / 20)
  , _dist_chance_bonus(MAX_BONUS_SPAWN_CHANCE / 2.0, MAX_BONUS_STD_DEV)
  , _dist_nb_bonus(1, _board_size / 50)
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
            if (!_game_ended) {
                _move_snakes();
                _check_player_state();
                _should_game_end();
                _respawn_food();
                _handle_bonus_food(loop_diff.count());
                if (!_paused) {
                    _game_length += loop_diff.count();
                }
            }
            _gfx_interface->clear();
            _gfx_interface->drawBoard();
            for (uint8_t i = 0; i < _nb_player; ++i) {
                _gfx_interface->drawSnake(_player[i].getSnakePosArray(),
                                          _player[i].getSnakeColorArray(),
                                          _player[i].getSnakeCurrentSize());
            }
            _gfx_interface->drawSnake(_obstacle.getSnakePosArray(),
                                      _obstacle.getSnakeColorArray(),
                                      _obstacle.getSnakeCurrentSize());
            _gfx_interface->drawSnake(_food.getSnakePosArray(),
                                      _food.getSnakeColorArray(),
                                      _food.getSnakeCurrentSize());
            _gfx_interface->drawSnake(_bonus_food.getSnakePosArray(),
                                      _bonus_food.getSnakeColorArray(),
                                      _bonus_food.getSnakeCurrentSize());
            _draw_stats_ui();
            _draw_interruption_ui();
            _gfx_interface->render();
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
    if (_paused) {
        return;
    }
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

// Win conditions handling
uint64_t
World::_current_used_board()
{
    return (_food.getSnakeCurrentSize() + _bonus_food.getSnakeCurrentSize() +
            _obstacle.getSnakeCurrentSize() +
            _player[PLAYER_1].getSnakeCurrentSize() +
            _player[PLAYER_2].getSnakeCurrentSize());
}

void
World::_check_player_state()
{
    if (_paused) {
        return;
    }

#ifdef NDEBUG
    for (uint8_t i = 0; i < _nb_player; ++i) {
        auto head_pos = _player[i].getSnakeHeadPos();

        // Check if player is inside board
        if (head_pos.x < 0 || head_pos.y < 0 || head_pos.x >= _params.board_w ||
            head_pos.y >= _params.board_h) {
            _player_win_con[i].out_of_map = 1;
            continue;
        }

        // Check if players touch obstacles
        if (_obstacle.isInsideSnake(head_pos)) {
            _player_win_con[i].touch_obstacle = 1;
            continue;
        }

        // Check if players touch itself
        if (_player[i].isSelfTouching()) {
            _player_win_con[i].touch_self = 1;
            continue;
        }
    }
    if (_nb_player > 1) {
        if (_player[PLAYER_1].isInsideSnake(
              _player[PLAYER_2].getSnakeHeadPos())) {
            _player_win_con[PLAYER_2].touch_player = 1;
        }
        if (_player[PLAYER_2].isInsideSnake(
              _player[PLAYER_1].getSnakeHeadPos())) {
            _player_win_con[PLAYER_1].touch_player = 1;
        }
    }
#endif
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

// Bonus food related
void
World::_respawn_food()
{
    if (!_food.getSnakeCurrentSize() && _current_used_board() < _board_size) {
        _generate_random_position(_food, glm::vec3(1.0f, 0.0f, 0.0f), 1);
    }
}

void
World::_handle_bonus_food(double elapsed_time)
{
    if (_paused) {
        return;
    }

    if (!_bonus_food_active) {
        if (_dist_chance_bonus(_mt_64) > _bonus_spawn_chance) {
            _bonus_food_active = 1;
            _current_bonus_food_timer = BONUS_DURATION;
            _generate_random_position(
              _bonus_food, glm::vec3(0.5f, 0.0f, 0.0f), _dist_nb_bonus(_mt_64));
        } else if (_bonus_spawn_chance) {
            --_bonus_spawn_chance;
        } else {
            _bonus_spawn_chance = MAX_BONUS_SPAWN_CHANCE;
        }
    } else {
        _current_bonus_food_timer -= elapsed_time;
        if (!_bonus_food.getSnakeCurrentSize() ||
            _current_bonus_food_timer <= 0.0) {
            _bonus_food_active = 0;
            _bonus_spawn_chance = MAX_BONUS_SPAWN_CHANCE;
            _bonus_food.init(
              glm::vec3{ 0.5f, 0.0f, 0.0f }, _params.board_w, _params.board_h);
        }
    }
}