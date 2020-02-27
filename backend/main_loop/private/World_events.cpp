#include <functional>

#include "World.hpp"

// Event handling functions
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
               &World::_set_sfml,          &World::_set_sdl,
               &World::_set_audio_sfml,    &World::_set_audio_none,
               &World::_mute_unmute };
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
        // TODO sound + theme handling
        _paused = !_paused;
        if (_game_ended) {
            _reset_game();
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
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_1] == Snake::DOWN) {
            return;
        }
        player[PLAYER_1].setSnakeDirection(Snake::UP);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_right()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_RIGHT]) {
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_1] == Snake::LEFT) {
            return;
        }
        player[PLAYER_1].setSnakeDirection(Snake::RIGHT);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_down()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_DOWN]) {
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_1] == Snake::UP) {
            return;
        }
        player[PLAYER_1].setSnakeDirection(Snake::DOWN);
        _event_timers.updated[P1] = 1;
        _event_timers.accept_event[P1] = 0;
    }
}

void
World::_p1_left()
{
    if (!_paused && _event_timers.accept_event[P1] &&
        _events[IGraphicTypes::P1_LEFT]) {
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_1] == Snake::RIGHT) {
            return;
        }
        player[PLAYER_1].setSnakeDirection(Snake::LEFT);
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
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_2] == Snake::DOWN) {
            return;
        }
        player[PLAYER_2].setSnakeDirection(Snake::UP);
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
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_2] == Snake::LEFT) {
            return;
        }
        player[PLAYER_2].setSnakeDirection(Snake::RIGHT);
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
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_2] == Snake::UP) {
            return;
        }
        player[PLAYER_2].setSnakeDirection(Snake::DOWN);
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
        auto &player = _board.updatePlayers();
        auto &previous_dir = _players.getSnakesPreviousDirection();

        if (previous_dir[PLAYER_2] == Snake::RIGHT) {
            return;
        }
        player[PLAYER_2].setSnakeDirection(Snake::LEFT);
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
        _clear_gfx_dyn_lib();
        _load_gfx_dyn_lib();
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
        _clear_gfx_dyn_lib();
        _load_gfx_dyn_lib();
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
        _clear_gfx_dyn_lib();
        _load_gfx_dyn_lib();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_set_audio_sfml()
{
    if (_params.sound_lib != SOUND_SFML && _event_timers.accept_event[SYSTEM] &&
        _events[IGraphicTypes::SET_AUDIO_SFML]) {
        _params.sound_lib = SOUND_SFML;
        _clear_audio_dyn_lib();
        _load_audio_dyn_lib();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_set_audio_none()
{
    if (_params.sound_lib != SOUND_NONE && _event_timers.accept_event[SYSTEM] &&
        _events[IGraphicTypes::SET_AUDIO_NONE]) {
        _params.sound_lib = SOUND_NONE;
        _clear_audio_dyn_lib();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}

void
World::_mute_unmute()
{
    if (_event_timers.accept_event[SYSTEM] &&
        _events[IGraphicTypes::MUTE_UNMUTE]) {

        _muted = !_muted;
        _set_audio_volume();
        _event_timers.accept_event[SYSTEM] = 0;
        _event_timers.updated[SYSTEM] = 1;
    }
}