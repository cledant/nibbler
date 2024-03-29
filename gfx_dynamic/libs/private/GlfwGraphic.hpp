#ifndef GLFWGRAPHIC_HPP
#define GLFWGRAPHIC_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "IGraphic.hpp"
#include "GLSnake.hpp"
#include "GLBoard.hpp"
#include "GLShader.hpp"
#include "GLFont.hpp"

class GlfwGraphic : public IGraphic
{
  public:
    GlfwGraphic();
    ~GlfwGraphic() override = default;
    GlfwGraphic(GlfwGraphic const &src) = delete;
    GlfwGraphic &operator=(GlfwGraphic const &rhs) = delete;
    GlfwGraphic(GlfwGraphic &&src) = delete;
    GlfwGraphic &operator=(GlfwGraphic &&rhs) = delete;

    void init(std::string const &home,
              int32_t w_square,
              int32_t h_square) override;
    void terminate() override;
    void createWindow(std::string &&name) override;
    void deleteWindow() override;
    uint8_t shouldClose() override;
    void triggerClose() override;
    void getEvents(
      std::array<uint8_t, IGraphicConstants::NB_EVENT> &events) override;
    void drawBoard() override;
    void drawSnake(
      std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size) override;
    void drawSnake(
      std::array<glm::ivec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size) override;
    void drawText(std::string const &str,
                  glm::vec3 const &color,
                  glm::vec2 const &pos,
                  float scale) override;
    void render() override;
    void clear() override;
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
        glm::vec2 screen_ratio;
    };

    struct Board
    {
        int32_t w;
        int32_t h;
        glm::vec2 gl_board_size;
        glm::vec2 gl_snake_board_size;
    };

    static constexpr float const VERTICAL_BORDER = 0.10f;

    std::string _home;
    Input _input;
    Win _win;
    Board _board;

    GLShader _gl_snake_shader;
    GLBoard _gl_board;
    GLSnake _gl_snake;
    GLFont _gl_font;

    void _initCallbacks();
    void _computeSquareRatio();
    void _computeBoardSize();
};

#endif