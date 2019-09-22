#include <iostream>

#include <sys/time.h>

#include "gfx_loader.hpp"

uint64_t constexpr SEC_IN_US = 1000000;
uint64_t constexpr SEC_IN_MS = 1000;
uint64_t constexpr KEYBOARD_TIMER = SEC_IN_US / 2;

uint64_t
get_time()
{
    struct timeval ts = { 0, 0 };

    gettimeofday(&ts, nullptr);
    return (ts.tv_sec * SEC_IN_US + ts.tv_usec);
}

void
get_events(uint8_t (&buffer)[IGraphic::NB_EVENT], IGraphic &gfx_interface)
{
    static uint64_t last_keyboard_pressed = 0;
    uint64_t time = get_time();
    uint8_t accept = ((time - last_keyboard_pressed) > KEYBOARD_TIMER);
    uint8_t update = accept;
    uint8_t updated_keys = 0;

    if (buffer[IGraphic::NibblerEvent::CLOSE_WIN] && accept) {
        gfx_interface.triggerClose();
        accept = 0;
        ++updated_keys;
    }
    if (buffer[IGraphic::NibblerEvent::TOGGLE_WIN] && accept) {
        gfx_interface.toggleFullscreen();
        accept = 0;
        ++updated_keys;
    }
    if (update && updated_keys) {
        last_keyboard_pressed = time;
    }
}

void
main_loop(IGraphic &gfx_interface)
{
    uint8_t buffer[IGraphic::NB_EVENT] = { 0 };

    while (!gfx_interface.shouldClose()) {
        get_events(buffer, gfx_interface);
        gfx_interface.draw();
        gfx_interface.render();
        gfx_interface.getEvents(buffer);
    }
    gfx_interface.deleteWindow();
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
        gfx_interface->init();
        gfx_interface->createWindow("Glfw_Nibbler");
        main_loop(*gfx_interface);
        gfx_interface->terminate();
        gfx_loader.getDeleter()(gfx_interface);
    } catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
        if (!gfx_interface) {
            gfx_interface->terminate();
            gfx_loader.getDeleter()(gfx_interface);
        }
    }
    return (0);
}