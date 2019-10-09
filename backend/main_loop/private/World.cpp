#include <chrono>

#include "World.hpp"

World::World(WorldParams const &params)
  : _params(params)
  , _gfx_loader()
  , _gfx_interface(nullptr)
  , _home()
  , _path_gfx_lib()
  , _snake_pos({ glm::uvec2{ 0, 0 },
                 glm::uvec2{ 0, 1 },
                 glm::uvec2{ 0, 2 },
                 glm::uvec2{ 0, 3 },
                 glm::uvec2{ 0, 4 } })
  , _snake_color({ glm::vec3{ 0.0, 1.0, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 },
                   glm::vec3{ 0.0, 0.5, 0.0 } })
  , _snake_size(5)
  , _is_init(0)
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
            std::runtime_error("Home not set");
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
        _gfx_interface->getEvents(_events);
        _get_events();
        _gfx_interface->clear();
        _gfx_interface->drawBoard();
        _gfx_interface->drawSnake(_snake_pos, _snake_color, _snake_size);
        _gfx_interface->render();
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
    // TODO Clean gettime fuction
    static auto last_keyboard_pressed =
      std::chrono::high_resolution_clock::now();
    auto time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = time - last_keyboard_pressed;
    uint8_t accept = (diff.count() > KEYBOARD_TIMER);
    uint8_t updated_keys = 0;

    if (_events[IGraphicTypes::NibblerEvent::CLOSE_WIN] && accept) {
        _gfx_interface->triggerClose();
        accept = 0;
        ++updated_keys;
    }
    if (_events[IGraphicTypes::NibblerEvent::TOGGLE_WIN] && accept) {
        _gfx_interface->toggleFullscreen();
        accept = 0;
        ++updated_keys;
    }
    if (updated_keys) {
        last_keyboard_pressed = time;
    }
}