#ifndef GLFWGRAPHIC_HPP
#define GLFWGRAPHIC_HPP

#include <array>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "IGraphic.hpp"

class GlfwGraphic : public IGraphic
{
  public:
    GlfwGraphic();
    ~GlfwGraphic() override = default;

    void init() override;
    void terminate() override;
    void createWindow(std::string &&name) override;
    void deleteWindow() override;
    uint8_t shouldClose() override;
    void triggerClose() override;
    void getEvents(uint8_t (&buffer)[NB_EVENT]) override;
    void draw() override;
    void render() override;
    void toggleFullscreen() override;

  private:
    void _initCallbacks();

    struct Input
    {
        std::array<uint8_t, 1024> keys;
    };

    struct Win
    {
        GLFWwindow *win;
        uint8_t fullscreen;
        int32_t w;
        int32_t h;
        int32_t w_viewport;
        int32_t h_viewport;
        std::string win_name;
    };

    Input _input;
    Win _win;
};

#endif