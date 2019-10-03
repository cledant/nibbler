#ifndef GLFWGRAPHIC_HPP
#define GLFWGRAPHIC_HPP

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
    void draw(Snake const &snake,
              enum SnakeType snakeType,
              enum DisplayType displayType) override;
    void render() override;
    void toggleFullscreen() override;

  private:
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

    struct GlfwSnake
    {
        uint32_t vbo_pos;
        uint32_t vbo_color;
        uint32_t vao;
        struct Snake snake;
    };

    Input _input;
    Win _win;
    std::array<GlfwSnake, MAX_SNAKES> _snake_array;

    void _initCallbacks();
    void _initSnake(GlfwSnake &snake);
    void _updateSnake(GlfwSnake &snake);
};

#endif