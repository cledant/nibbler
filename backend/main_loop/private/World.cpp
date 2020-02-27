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
  , _path_audio_lib()
  , _gfx_loader()
  , _audio_loader()
  , _gfx_interface(nullptr)
  , _audio_interface(nullptr)
  , _is_init(0)
  , _muted(0)
  , _loop_time_ref(std::chrono::high_resolution_clock::now())
  , _events()
  , _event_timers()
  , _players()
  , _board(params.board_w, params.board_h)
{
    _reset_game();
}

World::~World()
{
    _clear_gfx_dyn_lib();
    _clear_audio_dyn_lib();
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
        _path_audio_lib[AUDIO_SFML] =
          _home + "/.nibbler/nibbler_libs/libaudio_dyn_sfml.dylib";
#else
        _path_gfx_lib[GFX_GLFW] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
        _path_gfx_lib[GFX_SFML] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_sfml.so";
        _path_gfx_lib[GFX_SDL] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_sdl2.so";
        _path_audio_lib[SOUND_SFML] =
          _home + "/.nibbler/nibbler_libs/libaudio_dyn_sfml.so";
#endif
        _load_gfx_dyn_lib();
        _load_audio_dyn_lib();
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
                _players.moveSnakes(_board, _nb_player);
                _board.checkPlayerState(
                  _nb_player, _players.updatePlayersWinConditionStates());
                _should_game_end(_players, _board);
                _board.respawnFood();
                _board.handleBonusFood(loop_diff.count());
                _game_length += loop_diff.count();
            }
            if (_gfx_interface) {
                _gfx_interface->clear();
                _gfx_interface->drawBoard();
                _board.drawBoardElements(_nb_player, _gfx_interface);
                _board.drawBoardStat(_nb_player, _gfx_interface);
                _players.drawPlayerStats(_nb_player, _gfx_interface);
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
World::_load_gfx_dyn_lib()
{
    if (_params.gfx_lib == GFX_NONE) {
        return;
    }

    if (!_gfx_interface) {
        _gfx_loader.openLib(_path_gfx_lib[_params.gfx_lib]);
        _gfx_interface = _gfx_loader.getCreator()();
        _gfx_interface->init(_home, _params.board_w, _params.board_h);
        _gfx_interface->createWindow("Nibbler");
    }
}

void
World::_clear_gfx_dyn_lib()
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
World::_load_audio_dyn_lib()
{
    if (_params.sound_lib == SOUND_NONE) {
        return;
    }

    if (!_audio_interface) {
        _audio_loader.openLib(_path_audio_lib[_params.sound_lib]);
        _audio_interface = _audio_loader.getCreator()();
        _audio_interface->init(_home);
        _muted = 0;
        _set_audio_volume();
    }
}

void
World::_clear_audio_dyn_lib()
{
    if (_audio_interface) {
        _audio_interface->terminate();
        _audio_loader.getDeleter()(_audio_interface);
        _audio_loader.closeLib();
        _audio_interface = nullptr;
    }
}

// Game related
void
World::_should_game_end(Players &players, Board const &board)
{
    if (_game_ended) {
        return;
    }

    auto const &have_player_lost = players.havePlayersLost();
    if (have_player_lost[PLAYER_1] || have_player_lost[PLAYER_2]) {
        _game_ended = 1;
    } else if (board.isFullNoFood()) {
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
    _players.resetPlayers(_nb_player);
    _board.resetBoard(_params.obstacles, _nb_player);
    _paused = 0;
    _game_ended = 0;
    _game_length = 0.0f;
}

// UI
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
        _players.drawConclusion(_nb_player, _gfx_interface);
    }
}

void
World::_set_audio_volume()
{
    if (!_audio_interface) {
        return;
    }

    if (_muted) {
        _audio_interface->setEatSoundVolume(0.0);
        _audio_interface->setThemeVolume(0.0);
    } else {
        _audio_interface->setEatSoundVolume(50.0);
        _audio_interface->setThemeVolume(50.0);
    }
}