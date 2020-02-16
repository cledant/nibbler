#ifndef SDL2GRAPHIC_HPP
#define SDL2GRAPHIC_HPP

#include "SDL2/SDL.h"

#include "IGraphic.hpp"
#include "GLSnake.hpp"
#include "GLBoard.hpp"
#include "GLShader.hpp"

class SDL2Graphic : public IGraphic
{
  public:
    SDL2Graphic();
    ~SDL2Graphic() override = default;
    SDL2Graphic(SDL2Graphic const &src) = delete;
    SDL2Graphic &operator=(SDL2Graphic const &rhs) = delete;
    SDL2Graphic(SDL2Graphic &&src) = delete;
    SDL2Graphic &operator=(SDL2Graphic &&rhs) = delete;

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
    void render() override;
    void clear() override;
    void toggleFullscreen() override;

  private:
    struct Win
    {
        SDL_Window *win;
        SDL_Renderer *renderer;
        SDL_GLContext gl_context;
        std::string win_name;
        uint8_t fullscreen;
        int32_t w;
        int32_t h;
        int32_t w_viewport;
        int32_t h_viewport;
        glm::vec2 screen_ratio;
    };

    struct Board
    {
        int32_t w;
        int32_t h;
        glm::vec2 gl_board_size;
        glm::vec2 gl_snake_board_size;
    };

    static constexpr float VERTICAL_BORDER = 0.10f;

    std::string _home;
    Win _win;
    Board _board;

    uint8_t _should_close;

    GLShader _gl_snake_shader;
    GLBoard _gl_board;
    GLSnake _gl_snake;

    void _computeSquareRatio();
    void _computeBoardSize();
    void _createWindow();
};

#endif
