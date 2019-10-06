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

    // Display elmts init
    std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> pos = {
        glm::vec2{ 0.5, 0.5 },
        glm::vec2{ -0.5, -0.5 },
        glm::vec2{ 0.5, -0.5 },
        glm::vec2{ -0.5, 0.5 }
    };
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> color = {
        glm::vec3{ 1.0, 0.0, 0.0 },
        glm::vec3{ 0.0, 1.0, 0.0 },
        glm::vec3{ 0.0, 0.0, 1.0 },
        glm::vec3{ 1.0, 1.0, 1.0 },
    };
    uint32_t size = 4;

    while (!gfx_interface.shouldClose()) {
        get_events(buffer, gfx_interface);
        gfx_interface.clear();
        gfx_interface.draw(pos, color, IGraphicTypes::APPLES, size);
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
        gfx_loader.openLib(home + "/.nibbler/nibbler_libs/libgfx_dyn_glfw.so");
        gfx_interface = gfx_loader.getCreator()();
        gfx_interface->init(home, 10, 20);
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