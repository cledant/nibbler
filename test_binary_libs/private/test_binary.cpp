#include <iostream>
#include <chrono>

#include "gfx_loader.hpp"

double constexpr KEYBOARD_TIMER = 0.5;

void
get_events(uint8_t (&buffer)[IGraphicConstants::NB_EVENT],
           IGraphic &gfx_interface)
{
    static auto last_keyboard_pressed =
      std::chrono::high_resolution_clock::now();
    auto time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = time - last_keyboard_pressed;
    uint8_t accept = (diff.count() > KEYBOARD_TIMER);
    uint8_t updated_keys = 0;

    if (buffer[IGraphicTypes::NibblerEvent::CLOSE_WIN] && accept) {
        gfx_interface.triggerClose();
        accept = 0;
        ++updated_keys;
    }
    if (buffer[IGraphicTypes::NibblerEvent::TOGGLE_WIN] && accept) {
        gfx_interface.toggleFullscreen();
        accept = 0;
        ++updated_keys;
    }
    if (updated_keys) {
        last_keyboard_pressed = time;
    }
}

void
main_loop(IGraphic &gfx_interface)
{
    uint8_t buffer[IGraphicConstants::NB_EVENT] = { 0 };

    // Apple
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> apple_pos = {
        glm::uvec2{ 5, 5 },
    };
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> apple_color = {
        glm::vec3{ 1.0, 0.0, 0.0 },
    };
    uint32_t apple_size = 1;

    // Snake
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> snake_pos = {
        glm::uvec2{ 0, 0 }, glm::uvec2{ 0, 1 }, glm::uvec2{ 0, 2 },
        glm::uvec2{ 0, 3 }, glm::uvec2{ 0, 4 },
    };
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> snake_color = {
        glm::vec3{ 0.0, 1.0, 0.0 }, glm::vec3{ 0.0, 0.5, 0.0 },
        glm::vec3{ 0.0, 0.5, 0.0 }, glm::vec3{ 0.0, 0.5, 0.0 },
        glm::vec3{ 0.0, 0.5, 0.0 },
    };
    uint32_t snake_size = 5;
    while (!gfx_interface.shouldClose()) {
        get_events(buffer, gfx_interface);
        gfx_interface.clear();
        gfx_interface.drawBoard();
        gfx_interface.drawSnake(apple_pos, apple_color, apple_size);
        gfx_interface.drawSnake(snake_pos, snake_color, snake_size);
        gfx_interface.render();
        gfx_interface.getEvents(buffer);
    }
}

int
main()
{
    std::string home(getenv("HOME"));
    GfxLoader gfx_loader;
    IGraphic *gfx_interface = nullptr;

    if (home.empty()) {
        std::cout << "Home not set" << std::endl;
        return (0);
    }
    try {
#ifdef __APPLE__
        gfx_loader.openLib(home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.dylib");
#else
        gfx_loader.openLib(home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so");
#endif
        gfx_interface = gfx_loader.getCreator()();
        gfx_interface->init(home, 20, 10);
        gfx_interface->createWindow("Glfw_Nibbler");
        main_loop(*gfx_interface);
        gfx_interface->deleteWindow();
        gfx_interface->terminate();
        gfx_loader.getDeleter()(gfx_interface);
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
        if (gfx_interface) {
            gfx_interface->deleteWindow();
            gfx_interface->terminate();
            gfx_loader.getDeleter()(gfx_interface);
        }
    }
    return (0);
}