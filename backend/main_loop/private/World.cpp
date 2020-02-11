#include <functional>
#include <iostream>
#include <cstring>

#include "World.hpp"

World::World(WorldParams const &params)
  : _params(params)
  , _board_size(params.board_h * params.board_w)
  , _home()
  , _gfx_loader()
  , _gfx_interface(nullptr)
  , _path_gfx_lib()
  , _events()
  , _event_timers()
  , _player()
  , _win_con()
  , _game_ended(0)
  , _is_init(0)
  , _paused(0)
  , _nb_player(_params.game_type + 1)
  , _loop_time_ref(std::chrono::high_resolution_clock::now())
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
            _check_player_state();
            _should_game_end();
            if (!_game_ended) {
                _gfx_interface->clear();
                _gfx_interface->drawBoard();
                for (uint8_t i = 0; i < _nb_player; ++i) {
                    _gfx_interface->drawSnake(_player[i].getSnakePosArray(),
                                              _player[i].getSnakeColorArray(),
                                              _player[i].getSnakeCurrentSize());
                }
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
        _win_con[PLAYER_1].out_of_map = _player[PLAYER_1].moveSnake(Snake::UP);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_right()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_RIGHT]) {
        _win_con[PLAYER_1].out_of_map =
          _player[PLAYER_1].moveSnake(Snake::RIGHT);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_down()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_DOWN]) {
        _win_con[PLAYER_1].out_of_map =
          _player[PLAYER_1].moveSnake(Snake::DOWN);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_left()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_LEFT]) {
        _win_con[PLAYER_1].out_of_map =
          _player[PLAYER_1].moveSnake(Snake::LEFT);
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
        _win_con[PLAYER_2].out_of_map = _player[PLAYER_2].moveSnake(Snake::UP);
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
        _win_con[PLAYER_2].out_of_map =
          _player[PLAYER_2].moveSnake(Snake::RIGHT);
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
        _win_con[PLAYER_2].out_of_map =
          _player[PLAYER_2].moveSnake(Snake::DOWN);
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
        _win_con[PLAYER_2].out_of_map =
          _player[PLAYER_2].moveSnake(Snake::LEFT);
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
World::_check_player_state()
{
    // P1 Checks
    if (_player[PLAYER_1].getSnakeCurrentSize() == _board_size) {
        _win_con[PLAYER_1].filled_map = 1;
    }

    if (_params.game_type == TWO_PLAYER) {
        // P1 Checks
        _win_con[PLAYER_1].touch_player =
          _player[PLAYER_2].isInsideSnake(_player[PLAYER_1].getSnakeHeadPos());

        // P2 Checks
        if (_player[PLAYER_2].getSnakeCurrentSize() == _board_size) {
            _win_con[PLAYER_2].filled_map = 1;
        }
        _win_con[PLAYER_2].touch_player =
          _player[PLAYER_1].isInsideSnake(_player[PLAYER_2].getSnakeHeadPos());

        // Common checks
        if (_params.game_type == TWO_PLAYER &&
            _player[PLAYER_1].getSnakeHeadPos() ==
              _player[PLAYER_2].getSnakeHeadPos()) {
            _win_con[PLAYER_1].touch_snake_head = 1;
            _win_con[PLAYER_2].touch_snake_head = 1;
        }
    }
}

void
World::_should_game_end()
{
    if (!_game_ended) {
        WinCondition continue_game;
        std::memset(&continue_game, 0, sizeof(WinCondition));

        if (std::memcmp(
              &_win_con[PLAYER_1], &continue_game, sizeof(WinCondition)) ||
            std::memcmp(
              &_win_con[PLAYER_2], &continue_game, sizeof(WinCondition))) {
            _game_ended = 1;
            _paused = 1;
            _end_message();
        }
    }
}

void
World::_end_message()
{
    // TODO MORE Message types
    std::cout << "GAME OVER\nPress Space to restart !" << std::endl;
}

void
World::_reset_board()
{
    _event_timers.timer_values = { SYSTEM_TIMER_SECONDS,
                                   DEFAULT_SNAKE_TIMER_SECONDS,
                                   DEFAULT_SNAKE_TIMER_SECONDS };
    _event_timers.time_ref = { std::chrono::high_resolution_clock::now(),
                               std::chrono::high_resolution_clock::now(),
                               std::chrono::high_resolution_clock::now() };
    if (_params.game_type == ONE_PLAYER) {
        _player[PLAYER_1].init(
          glm::uvec2{ _params.board_w / 2, _params.board_h / 2 },
          glm::vec3{ 0.0f, 1.0f, 0.0f },
          _params.board_w,
          _params.board_h);
    } else {
        _player[PLAYER_1].init(
          glm::uvec2{ _params.board_w / 4, _params.board_h / 2 },
          glm::vec3{ 0.0f, 1.0f, 0.0f },
          _params.board_w,
          _params.board_h);
        _player[PLAYER_2].init(
          glm::uvec2{ 3 * _params.board_w / 4, _params.board_h / 2 },
          glm::vec3{ 0.0f, 0.0f, 1.0f },
          _params.board_w,
          _params.board_h);
    }
    std::memset(&_win_con, 0, sizeof(WinCondition) * NB_PLAYER_MAX);
    std::memset(&_events, 0, sizeof(uint8_t) * IGraphicConstants::NB_EVENT);
    _paused = 0;
    _game_ended = 0;
}