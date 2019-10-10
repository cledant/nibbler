#include "World.hpp"

World::World(WorldParams const &params)
  : _params(params)
  , _home()
  , _gfx_loader()
  , _gfx_interface(nullptr)
  , _path_gfx_lib()
  , _events()
  , _snake_pos({ glm::uvec2{ 5, 5 } })
  , _snake_color({ glm::vec3{ 0.0, 1.0, 0.0 } })
  , _snake_size(1)
  , _is_init(0)
  , _loop_time_ref(std::chrono::high_resolution_clock::now())
  , _system_time_ref(std::chrono::high_resolution_clock::now())
  , _snake_time_ref(std::chrono::high_resolution_clock::now())
{}

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
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
        _path_gfx_lib[GFX_SFML] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
        _path_gfx_lib[GFX_SDL] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
#else
        _path_gfx_lib[GFX_GLFW] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
        _path_gfx_lib[GFX_SFML] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
        _path_gfx_lib[GFX_SDL] =
          _home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so";
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
            _get_events();
            _gfx_interface->clear();
            _gfx_interface->drawBoard();
            _gfx_interface->drawSnake(_snake_pos, _snake_color, _snake_size);
            _gfx_interface->render();
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
    }
}

void
World::_get_events()
{
    // TODO Use a for loop and functions pointers
    auto now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> system_diff = now - _system_time_ref;
    std::chrono::duration<double> snake_diff = now - _snake_time_ref;

    uint8_t system_accept = (system_diff.count() > SYSTEM_TIMER_SECONDS);
    uint8_t system_updated = 0;

    uint8_t snake_accept = (snake_diff.count() > DEFAULT_SNAKE_TIMER_SECONDS);
    uint8_t snake_updated = 0;

    if (_events[IGraphicTypes::NibblerEvent::CLOSE_WIN] && system_accept) {
        _gfx_interface->triggerClose();
        system_accept = 0;
        system_updated = 1;
    }
    if (_events[IGraphicTypes::NibblerEvent::TOGGLE_WIN] && system_accept) {
        _gfx_interface->toggleFullscreen();
        system_accept = 0;
        system_updated = 1;
    }
    if (snake_accept && _events[IGraphicTypes::P1_UP] && _snake_pos[0].y > 0) {
        _snake_pos[0].y -= 1;
        snake_updated = 1;
    }
    if (snake_accept && _events[IGraphicTypes::P1_LEFT] &&
        _snake_pos[0].x > 0) {
        _snake_pos[0].x -= 1;
        snake_updated = 1;
    }
    if (snake_accept && _events[IGraphicTypes::P1_DOWN] &&
        _snake_pos[0].y < _params.board_h - 1) {
        _snake_pos[0].y += 1;
        snake_updated = 1;
    }
    if (snake_accept && _events[IGraphicTypes::P1_RIGHT] &&
        _snake_pos[0].x < _params.board_w - 1) {
        _snake_pos[0].x += 1;
        snake_updated = 1;
    }
    if (system_updated) {
        _system_time_ref = now;
    }
    if (snake_updated) {
        _snake_time_ref = now;
    }
}