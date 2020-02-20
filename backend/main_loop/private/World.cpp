#include <functional>
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
  , _player_previous_frame_dir()
  , _player_score()
  , _player_mvt_timer()
  , _rd()
  , _mt_64(_rd())
  , _dist_board_w(1, _params.board_w - 2)
  , _dist_board_h(1, _params.board_h - 2)
  , _dist_obstacle(0, _board_size / 20)
{
    _reset_board();
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
            _move_snakes();
            _check_player_state();
            _should_game_end();
            if (!_food.getSnakeCurrentSize()) {
                _generate_random_position(
                  _food, glm::vec3(1.0f, 0.0f, 0.0f), 1);
            }
            if (!_game_ended) {
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
                _gfx_interface->render();
            }
            _loop_time_ref = now;
        }
    }
}

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

void
World::_interpret_events()
{
    static const std::array<void (World::*)(), IGraphicConstants::NB_EVENT>
      func = { &World::_close_win_event,   &World::_pause,
               &World::_toggle_fullscreen, &World::_p1_up,
               &World::_p1_right,          &World::_p1_down,
               &World::_p1_left,           &World::_p2_up,
               &World::_p2_right,          &World::_p2_down,
               &World::_p2_left,           &World::_set_glfw,
               &World::_set_sfml,          &World::_set_sdl };
    auto now = std::chrono::high_resolution_clock::now();

    for (uint8_t i = 0; i < NB_EVENT_TIMER_TYPES; ++i) {
        std::chrono::duration<double> time_diff =
          now - _event_timers.time_ref[i];
        _event_timers.accept_event[i] =
          (time_diff.count() > _event_timers.timer_values[i]);
    }
    for (auto const &it : func) {
        std::invoke(it, this);
    }
    for (uint8_t i = 0; i < NB_EVENT_TIMER_TYPES; ++i) {
        if (_event_timers.updated[i]) {
            _event_timers.time_ref[i] = now;
        }
        _event_timers.updated[i] = 0;
    }
}

void
World::_close_win_event()
{
    if (_events[IGraphicTypes::NibblerEvent::CLOSE_WIN] &&
        _event_timers.accept_event[SYSTEM]) {
        _gfx_interface->triggerClose();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_pause()
{
    if (_events[IGraphicTypes::NibblerEvent::PAUSE] &&
        _event_timers.accept_event[SYSTEM]) {
        _paused = !_paused;
        if (_game_ended) {
            _reset_board();
        }
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_toggle_fullscreen()
{
    if (_events[IGraphicTypes::NibblerEvent::TOGGLE_WIN] &&
        _event_timers.accept_event[SYSTEM]) {
        _gfx_interface->toggleFullscreen();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_p1_up()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_UP]) {
        if (_player_previous_frame_dir[PLAYER_1] == Snake::DOWN) {
            return;
        }
        _player[PLAYER_1].setSnakeDirection(Snake::UP);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_right()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_RIGHT]) {
        if (_player_previous_frame_dir[PLAYER_1] == Snake::LEFT) {
            return;
        }
        _player[PLAYER_1].setSnakeDirection(Snake::RIGHT);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_down()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_DOWN]) {
        if (_player_previous_frame_dir[PLAYER_1] == Snake::UP) {
            return;
        }
        _player[PLAYER_1].setSnakeDirection(Snake::DOWN);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_left()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_LEFT]) {
        if (_player_previous_frame_dir[PLAYER_1] == Snake::RIGHT) {
            return;
        }
        _player[PLAYER_1].setSnakeDirection(Snake::LEFT);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p2_up()
{
    if (!_paused && _event_timers.accept_event[P2] &&
        _events[IGraphicTypes::P2_UP] &&
        _params.game_type == Gametype::TWO_PLAYER) {
        if (_player_previous_frame_dir[PLAYER_2] == Snake::DOWN) {
            return;
        }
        _player[PLAYER_2].setSnakeDirection(Snake::UP);
        _event_timers.updated[P2] = 1;
        _event_timers.accept_event[P2] = 0;
    }
}

void
World::_p2_right()
{
    if (!_paused && _event_timers.accept_event[P2] &&
        _events[IGraphicTypes::P2_RIGHT] &&
        _params.game_type == Gametype::TWO_PLAYER) {
        if (_player_previous_frame_dir[PLAYER_2] == Snake::LEFT) {
            return;
        }
        _player[PLAYER_2].setSnakeDirection(Snake::RIGHT);
        _event_timers.updated[P2] = 1;
        _event_timers.accept_event[P2] = 0;
    }
}

void
World::_p2_down()
{
    if (!_paused && _event_timers.accept_event[P2] &&
        _events[IGraphicTypes::P2_DOWN] &&
        _params.game_type == Gametype::TWO_PLAYER) {
        if (_player_previous_frame_dir[PLAYER_2] == Snake::UP) {
            return;
        }
        _player[PLAYER_2].setSnakeDirection(Snake::DOWN);
        _event_timers.updated[P2] = 1;
        _event_timers.accept_event[P2] = 0;
    }
}

void
World::_p2_left()
{
    if (!_paused && _event_timers.accept_event[P2] &&
        _events[IGraphicTypes::P2_LEFT] &&
        _params.game_type == Gametype::TWO_PLAYER) {
        if (_player_previous_frame_dir[PLAYER_2] == Snake::RIGHT) {
            return;
        }
        _player[PLAYER_2].setSnakeDirection(Snake::LEFT);
        _event_timers.updated[P2] = 1;
        _event_timers.accept_event[P2] = 0;
    }
}

void
World::_set_glfw()
{
    if (_params.gfx_lib != GFX_GLFW && _event_timers.accept_event[SYSTEM] &&
        _events[IGraphicTypes::SET_GLFW]) {
        _params.gfx_lib = GFX_GLFW;
        _paused = 1;
        _clear_dyn_lib();
        _load_dyn_lib();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_set_sfml()
{
    if (_params.gfx_lib != GFX_SFML && _event_timers.accept_event[SYSTEM] &&
        _events[IGraphicTypes::SET_SFML]) {
        _params.gfx_lib = GFX_SFML;
        _paused = 1;
        _clear_dyn_lib();
        _load_dyn_lib();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_set_sdl()
{
    if (_params.gfx_lib != GFX_SDL && _event_timers.accept_event[SYSTEM] &&
        _events[IGraphicTypes::SET_SDL]) {
        _params.gfx_lib = GFX_SDL;
        _paused = 1;
        _clear_dyn_lib();
        _load_dyn_lib();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

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
                _player_score[i] += 1;
                _player_mvt_timer.timer_values[i] =
                  _player_timers[_player_score[i]];
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

uint64_t
World::_current_used_board()
{
    return (_food.getSnakeCurrentSize() + _obstacle.getSnakeCurrentSize() +
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

    uint64_t p1_lost = memcmp(
      &_player_win_con[PLAYER_1], &not_lost, sizeof(struct WinCondition));
    uint64_t p2_lost = memcmp(
      &_player_win_con[PLAYER_2], &not_lost, sizeof(struct WinCondition));

    if (p1_lost && p2_lost) {
        _game_ended = 1;
        std::cout << "Draw : Both players lost" << std::endl;
    } else if (p1_lost) {
        _game_ended = 1;
        std::cout << "Player 1 lost" << std::endl;
    } else if (p2_lost) {
        _game_ended = 1;
        std::cout << "Player 2 lost" << std::endl;
    } else if (_nb_player == 1 &&
               (_current_used_board() - _food.getSnakeCurrentSize()) ==
                 _board_size) {
        _game_ended = 1;
        std::cout << "Player 1 won" << std::endl;
    } else if (_nb_player == 2 &&
               (_current_used_board() - _food.getSnakeCurrentSize()) ==
                 _board_size) {
        _game_ended = 1;
        std::cout << "Draw : Both players alive" << std::endl;
    }
}

void
World::_generate_random_position(Snake &target,
                                 glm::vec3 const &color,
                                 uint64_t nb_to_add)
{
    for (uint64_t i = 0; i < nb_to_add; ++i) {
        uint8_t valid_position = 0;

        while (!valid_position) {
            auto new_obstacle =
              glm::ivec2(_dist_board_w(_mt_64), _dist_board_h(_mt_64));

            if (!_player[PLAYER_1].isInsideSnake(new_obstacle) &&
                !_player[PLAYER_2].isInsideSnake(new_obstacle) &&
                !_obstacle.isInsideSnake(new_obstacle) &&
                !_food.isInsideSnake(new_obstacle)) {
                valid_position = 1;
                target.addToSnake(new_obstacle, color);
            }
        }
    }
}

void
World::_reset_board()
{
    _event_timers.timer_values = { SYSTEM_TIMER_SECONDS,
                                   PLAYER_TIMER_SECONDS,
                                   PLAYER_TIMER_SECONDS };
    _event_timers.time_ref = { std::chrono::high_resolution_clock::now(),
                               std::chrono::high_resolution_clock::now(),
                               std::chrono::high_resolution_clock::now() };
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
    _obstacle.init(
      glm::vec3{ 0.33f, 0.33f, 0.33f }, _params.board_w, _params.board_h);
    _food.init(glm::vec3{ 1.0f, 0.0f, 0.0f }, _params.board_w, _params.board_h);
    if (_params.obstacles) {
        _generate_random_position(
          _obstacle, glm::vec3(0.33f), _dist_obstacle(_mt_64));
    }
    std::memset(&_player_win_con, 0, sizeof(WinCondition) * NB_PLAYER_MAX);
    std::memset(&_events, 0, sizeof(uint8_t) * IGraphicConstants::NB_EVENT);
    std::memset(&_player_previous_frame_dir,
                Snake::UP,
                sizeof(enum Snake::snakeDirection) * NB_PLAYER_MAX);
    std::memset(&_player_score, 0, sizeof(uint64_t) * NB_PLAYER_MAX);
    _paused = 0;
    _game_ended = 0;
}