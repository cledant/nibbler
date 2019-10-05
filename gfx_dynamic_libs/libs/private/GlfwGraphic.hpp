#ifndef GLFWGRAPHIC_HPP
#define GLFWGRAPHIC_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "IGraphic.hpp"
#include "GLSnake.hpp"

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
    void getEvents(uint8_t (&buffer)[IGraphicConstants::NB_EVENT]) override;
    void draw(
      std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size) override;
    void draw(
      std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size) override;
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

    Input _input;
    Win _win;
    //std::array<GLSnake, IGraphicConstants::MAX_SNAKES> _snake_array;

    void _initCallbacks();
};

#endif