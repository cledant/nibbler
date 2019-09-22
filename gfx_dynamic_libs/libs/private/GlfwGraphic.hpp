#ifndef GLFWGRAPHIC_HPP
#define GLFWGRAPHIC_HPP

#include <array>

#include "GLFW/glfw3.h"

#include "IGraphic.hpp"

class GlfwGraphic : public IGraphic
{
  public:
    GlfwGraphic() = default;
    ~GlfwGraphic() override = default;

    void init() override;
    void terminate() override;
    void createWindow(int32_t w, int32_t h, std::string &&name) override;
    void deleteWindow() override;
    uint8_t shouldClose() override;
    void getEvents(uint8_t (&buffer)[NB_EVENT]) override;

  private:
    static void error_callback(int error, char const *description);

    struct Input
    {
        std::array<uint8_t, 1024> keys;
    };

    struct Win
    {
        GLFWwindow *win;
        int32_t w;
        int32_t h;
        std::string win_name;
    };

    Input _input;
    Win _win;
};

#endif