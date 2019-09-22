#include <iostream>
#include <stdexcept>
#include <string>

#include "GlfwGraphic.hpp"

void
GlfwGraphic::init()
{
    glfwSetErrorCallback(error_callback);
    glfwInit();
}

void
GlfwGraphic::terminate()
{
    glfwTerminate();
}

void
GlfwGraphic::createWindow(int32_t w, int32_t h, std::string &&name)
{
    _win.h = h;
    _win.w = w;
    _win.win_name = name;
}

void
GlfwGraphic::deleteWindow()
{}

void GlfwGraphic::getEvents(uint8_t (&buffer)[NB_EVENT])
{
    (void)buffer;
}

uint8_t
GlfwGraphic::shouldClose()
{
    return (0);
}

void
GlfwGraphic::error_callback(int error, char const *description)
{
    (void)error;
    std::string str_err("Glfw : ");
    str_err.append(description);
    throw std::runtime_error(str_err);
}

extern "C" IGraphic *
creator()
{
    return (new GlfwGraphic());
}

extern "C" void
deleter(IGraphic *gfx)
{
    delete gfx;
}