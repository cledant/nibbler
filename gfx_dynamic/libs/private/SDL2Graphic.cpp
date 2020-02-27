#include <stdexcept>

#include "SDL2Graphic.hpp"

SDL2Graphic::SDL2Graphic()
  : _win()
  , _board()
  , _should_close(0)
{
    _win.win = nullptr;
    _win.renderer = nullptr;
    _win.gl_context = nullptr;
    _win.fullscreen = 0;
    _win.w = 0;
    _win.h = 0;
    _win.w_viewport = 0;
    _win.h_viewport = 0;
    _win.screen_ratio = glm::vec2(1.0f);
    _board.h = 0;
    _board.w = 0;
    _board.gl_snake_board_size = glm::vec2(1.0f);
    _board.gl_board_size = glm::vec2(1.0f);
}

void
SDL2Graphic::init(const std::string &home, int32_t w_square, int32_t h_square)
{
    _board.w = w_square;
    _board.h = h_square;
    _home = home;
    if (SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("SDL2 : failed to init");
    }
}

void
SDL2Graphic::terminate()
{
    SDL_Quit();
}

void
SDL2Graphic::createWindow(std::string &&name)
{
    _win.win_name = std::move(name);
    _createWindow();
}

void
SDL2Graphic::deleteWindow()
{
    _gl_board.clear();
    _gl_snake.clear();
    _gl_snake_shader.clear();
    _gl_font.clear();
    if (_win.gl_context) {
        SDL_GL_DeleteContext(_win.gl_context);
        _win.gl_context = nullptr;
    }
    if (_win.renderer) {
        SDL_DestroyRenderer(_win.renderer);
        _win.renderer = nullptr;
    }
    if (_win.win) {
        SDL_DestroyWindow(_win.win);
        _win.win = nullptr;
    }
}

uint8_t
SDL2Graphic::shouldClose()
{
    return (_should_close);
}

void
SDL2Graphic::triggerClose()
{
    _should_close = 1;
}

void
SDL2Graphic::getEvents(std::array<uint8_t, IGraphicConstants::NB_EVENT> &events)
{
    auto buffer = events.data();
    SDL_PumpEvents();
    auto const *key_state = SDL_GetKeyboardState(nullptr);

    buffer[IGraphicTypes::NibblerEvent::CLOSE_WIN] =
      key_state[SDL_SCANCODE_ESCAPE];
    buffer[IGraphicTypes::NibblerEvent::PAUSE] = key_state[SDL_SCANCODE_SPACE];
    buffer[IGraphicTypes::NibblerEvent::TOGGLE_WIN] =
      key_state[SDL_SCANCODE_F4];
    buffer[IGraphicTypes::NibblerEvent::P1_UP] = key_state[SDL_SCANCODE_W];
    buffer[IGraphicTypes::NibblerEvent::P1_RIGHT] = key_state[SDL_SCANCODE_D];
    buffer[IGraphicTypes::NibblerEvent::P1_DOWN] = key_state[SDL_SCANCODE_S];
    buffer[IGraphicTypes::NibblerEvent::P1_LEFT] = key_state[SDL_SCANCODE_A];
    buffer[IGraphicTypes::NibblerEvent::P2_UP] = key_state[SDL_SCANCODE_UP];
    buffer[IGraphicTypes::NibblerEvent::P2_RIGHT] =
      key_state[SDL_SCANCODE_RIGHT];
    buffer[IGraphicTypes::NibblerEvent::P2_DOWN] = key_state[SDL_SCANCODE_DOWN];
    buffer[IGraphicTypes::NibblerEvent::P2_LEFT] = key_state[SDL_SCANCODE_LEFT];
    buffer[IGraphicTypes::NibblerEvent::SET_GLFW] = key_state[SDL_SCANCODE_F1];
    buffer[IGraphicTypes::NibblerEvent::SET_SFML] = key_state[SDL_SCANCODE_F2];
    buffer[IGraphicTypes::NibblerEvent::SET_SDL] = key_state[SDL_SCANCODE_F3];
    buffer[IGraphicTypes::NibblerEvent::SET_AUDIO_SFML] =
      key_state[SDL_SCANCODE_F5];
    buffer[IGraphicTypes::NibblerEvent::SET_AUDIO_NONE] =
      key_state[SDL_SCANCODE_F6];
    buffer[IGraphicTypes::NibblerEvent::MUTE_UNMUTE] =
      key_state[SDL_SCANCODE_F7];
}

void
SDL2Graphic::drawBoard()
{
    _gl_snake_shader.use();
    _gl_snake_shader.setVec2("uniform_scale", _board.gl_board_size);
    glBindVertexArray(_gl_board.getVao());
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}

void
SDL2Graphic::drawSnake(
  std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
  std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
  uint32_t size)
{
    _gl_snake_shader.use();
    glBindVertexArray(_gl_snake.getVao());
    _gl_snake_shader.setVec2("uniform_scale", _board.gl_snake_board_size);
    _gl_snake.updateVbo(pos, color, size);
    glDrawArrays(GL_POINTS, 0, size);
    glBindVertexArray(0);
}

void
SDL2Graphic::drawSnake(
  std::array<glm::ivec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
  std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
  uint32_t size)
{
    _gl_snake_shader.use();
    glBindVertexArray(_gl_snake.getVao());
    _gl_snake_shader.setVec2("uniform_scale", _board.gl_snake_board_size);
    _gl_snake.updateVbo(
      pos, _board.gl_snake_board_size, _board.w, _board.h, color, size);
    glDrawArrays(GL_POINTS, 0, size);
    glBindVertexArray(0);
}

void
SDL2Graphic::drawText(std::string const &str,
                      glm::vec3 const &color,
                      glm::vec2 const &pos,
                      float scale)
{
    _gl_font.drawText(str, color, pos, scale);
}

void
SDL2Graphic::render()
{
    SDL_GL_SwapWindow(_win.win);
}

void
SDL2Graphic::clear()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
SDL2Graphic::toggleFullscreen()
{
    _win.fullscreen = !_win.fullscreen;
    deleteWindow();
    _createWindow();
}

void
SDL2Graphic::_computeSquareRatio()
{
    float w =
      (_win.w_viewport >= _win.h_viewport) ? _win.w_viewport : _win.h_viewport;
    float h =
      (_win.w_viewport >= _win.h_viewport) ? _win.h_viewport : _win.w_viewport;

    _win.screen_ratio = (_win.w_viewport >= _win.h_viewport)
                          ? glm::vec2(h / w, 1.0f)
                          : glm::vec2(1.0f, h / w);
}

void
SDL2Graphic::_computeBoardSize()
{
    _board.gl_board_size = _win.screen_ratio * (1.0f - VERTICAL_BORDER);

    float largest = (_board.w >= _board.h) ? _board.w : _board.h;
    _board.gl_snake_board_size = _board.gl_board_size / largest;

    _board.gl_board_size =
      glm::vec2(_board.w, _board.h) * _board.gl_snake_board_size;
}

void
SDL2Graphic::_createWindow()
{
    uint32_t flags = SDL_WINDOW_OPENGL;

    if (_win.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    _win.win = SDL_CreateWindow(_win.win_name.c_str(),
                                100,
                                100,
                                IGraphicConstants::WIN_W,
                                IGraphicConstants::WIN_H,
                                flags);
    if (!_win.win) {
        throw std::runtime_error("SDL2 : failed to create window");
    }
    if (!(_win.renderer =
            SDL_CreateRenderer(_win.win, -1, SDL_RENDERER_ACCELERATED))) {
        SDL_DestroyWindow(_win.win);
        throw std::runtime_error("SDL2 : failed to create renderer");
    }
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    if (!(_win.gl_context = SDL_GL_CreateContext(_win.win))) {
        SDL_DestroyWindow(_win.win);
        SDL_DestroyRenderer(_win.renderer);
        throw std::runtime_error("SDL2 : failed to create gl context");
    }
    SDL_GL_SetSwapInterval(0);
    SDL_GetWindowSize(_win.win, &_win.w_viewport, &_win.h_viewport);
    _win.h = _win.h_viewport;
    _win.w = _win.w_viewport;
    _computeSquareRatio();
    _computeBoardSize();

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        throw std::runtime_error("GLAD not loaded");
    }
    glViewport(0, 0, _win.w_viewport, _win.h_viewport);
    _gl_snake_shader.init(
      _home + "/.nibbler/nibbler_shaders/draw_rectangle/draw_rectangle_vs.glsl",
      _home + "/.nibbler/nibbler_shaders/draw_rectangle/draw_rectangle_gs.glsl",
      _home + "/.nibbler/nibbler_shaders/draw_rectangle/draw_rectangle_fs.glsl",
      "draw_rectangle");
    _gl_snake.init();
    _gl_board.init();
    _gl_font.init(_home + "/.nibbler/nibbler_fonts/Roboto-Light.ttf",
                  _home + "/.nibbler/nibbler_shaders/font/font_vs.glsl",
                  _home + "/.nibbler/nibbler_shaders/font/font_fs.glsl",
                  glm::vec2(_win.w, _win.h),
                  48);
}

extern "C" IGraphic *
creator()
{
    return (new SDL2Graphic());
}

extern "C" void
deleter(IGraphic *gfx)
{
    delete gfx;
}