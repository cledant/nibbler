#include <iostream>
#include <stdexcept>
#include <string>
#include <cstring>

#include "GlfwGraphic.hpp"

#define THIS_WIN_PTR static_cast<GlfwGraphic *>(glfwGetWindowUserPointer(win))

GlfwGraphic::GlfwGraphic()
  : _input()
  , _win()
  , _board()
{
    memset(_input.keys.data(), 0, sizeof(uint8_t) * _input.keys.size());
    _win.win = nullptr;
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
GlfwGraphic::init(std::string const &home, int32_t w_square, int32_t h_square)
{
    _board.w = w_square;
    _board.h = h_square;
    _home = home;
    if (!glfwInit()) {
        throw std::runtime_error("Glfw : failed to init");
    }
}

void
GlfwGraphic::terminate()
{
    glfwTerminate();
}

void
GlfwGraphic::createWindow(std::string &&name)
{
    if (!_win.win) {
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 8);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        _win.win_name = name;
        _win.win =
          glfwCreateWindow(640, 480, _win.win_name.c_str(), nullptr, nullptr);
        if (!_win.win) {
            throw std::runtime_error("Glfw : failed to create window");
        }
        glfwSetInputMode(_win.win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetWindowPos(_win.win, 100, 100);
        glfwSetWindowUserPointer(_win.win, this);
        _initCallbacks();
        glfwMakeContextCurrent(_win.win);
        glfwSwapInterval(0);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("GLAD not loaded");
        }
        glfwSetWindowSize(
          _win.win, IGraphicConstants::WIN_W, IGraphicConstants::WIN_H);
        _gl_snake_shader.init(
          _home +
            "/.nibbler/nibbler_shaders/draw_rectangle/draw_rectangle_vs.glsl",
          _home +
            "/.nibbler/nibbler_shaders/draw_rectangle/draw_rectangle_gs.glsl",
          _home +
            "/.nibbler/nibbler_shaders/draw_rectangle/draw_rectangle_fs.glsl",
          "draw_rectangle");
        _gl_snake.init();
        _gl_board.init();
    }
}

void
GlfwGraphic::deleteWindow()
{
    _gl_board.clear();
    _gl_snake.clear();
    _gl_snake_shader.clear();
    if (!_win.win) {
        glfwDestroyWindow(_win.win);
        _win.win = nullptr;
    }
}

uint8_t
GlfwGraphic::shouldClose()
{
    return (glfwWindowShouldClose(_win.win));
}

void
GlfwGraphic::triggerClose()
{
    glfwSetWindowShouldClose(_win.win, 1);
}

void
GlfwGraphic::getEvents(std::array<uint8_t, IGraphicConstants::NB_EVENT> &events)
{
    glfwPollEvents();
    auto buffer = events.data();

    buffer[IGraphicTypes::NibblerEvent::CLOSE_WIN] =
      _input.keys[GLFW_KEY_ESCAPE];
    buffer[IGraphicTypes::NibblerEvent::PAUSE] = _input.keys[GLFW_KEY_SPACE];
    buffer[IGraphicTypes::NibblerEvent::TOGGLE_WIN] = _input.keys[GLFW_KEY_F5];
    buffer[IGraphicTypes::NibblerEvent::P1_UP] = _input.keys[GLFW_KEY_W];
    buffer[IGraphicTypes::NibblerEvent::P1_RIGHT] = _input.keys[GLFW_KEY_D];
    buffer[IGraphicTypes::NibblerEvent::P1_DOWN] = _input.keys[GLFW_KEY_S];
    buffer[IGraphicTypes::NibblerEvent::P1_LEFT] = _input.keys[GLFW_KEY_A];
    buffer[IGraphicTypes::NibblerEvent::P2_UP] = _input.keys[GLFW_KEY_UP];
    buffer[IGraphicTypes::NibblerEvent::P2_RIGHT] = _input.keys[GLFW_KEY_RIGHT];
    buffer[IGraphicTypes::NibblerEvent::P2_DOWN] = _input.keys[GLFW_KEY_DOWN];
    buffer[IGraphicTypes::NibblerEvent::P2_LEFT] = _input.keys[GLFW_KEY_LEFT];
    buffer[IGraphicTypes::NibblerEvent::SET_GLFW] = _input.keys[GLFW_KEY_F1];
    buffer[IGraphicTypes::NibblerEvent::SET_SFML] = _input.keys[GLFW_KEY_F2];
    buffer[IGraphicTypes::NibblerEvent::SET_SDL] = _input.keys[GLFW_KEY_F3];
}

void
GlfwGraphic::drawBoard()
{
    _gl_snake_shader.use();
    _gl_snake_shader.setVec2("uniform_scale", _board.gl_board_size);
    glBindVertexArray(_gl_board.getVao());
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}

void
GlfwGraphic::drawSnake(
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
GlfwGraphic::drawSnake(
  std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
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
GlfwGraphic::render()
{
    glfwSwapBuffers(_win.win);
}

void
GlfwGraphic::clear()
{
    glClearColor(0.086f, 0.317f, 0.427f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
GlfwGraphic::toggleFullscreen()
{
    _win.fullscreen = !_win.fullscreen;

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        throw std::runtime_error("Glfw : No primary monitor");
    }

    int32_t count;
    GLFWvidmode const *mode = glfwGetVideoModes(monitor, &count);
    if (!mode) {
        throw std::runtime_error("Glfw : failed to fetch monitor mode");
    }

    if (_win.fullscreen) {
        glfwSetWindowMonitor(
          _win.win, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(_win.win,
                             nullptr,
                             100,
                             100,
                             IGraphicConstants::WIN_W,
                             IGraphicConstants::WIN_H,
                             GLFW_DONT_CARE);
    }
}

void
GlfwGraphic::_initCallbacks()
{
    // Input
    auto input_callback =
      [](GLFWwindow *win, int key, int scancode, int action, int mods) {
          static_cast<void>(scancode);
          static_cast<void>(mods);
          if (key >= 0 && key < 1024) {
              if (action == GLFW_PRESS) {
                  THIS_WIN_PTR->_input.keys[key] = 1;
              } else if (action == GLFW_RELEASE) {
                  THIS_WIN_PTR->_input.keys[key] = 0;
              }
          }
      };
    glfwSetKeyCallback(_win.win, input_callback);

    // Close
    auto close_callback = [](GLFWwindow *win) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
    };
    glfwSetWindowCloseCallback(_win.win, close_callback);

    // Window
    auto window_size_callback = [](GLFWwindow *win, int w, int h) {
        THIS_WIN_PTR->_win.h = h;
        THIS_WIN_PTR->_win.w = w;
    };
    glfwSetWindowSizeCallback(_win.win, window_size_callback);

    // Framebuffer
    auto framebuffer_size_callback = [](GLFWwindow *win, int w, int h) {
        THIS_WIN_PTR->_win.w_viewport = w;
        THIS_WIN_PTR->_win.h_viewport = h;
        glViewport(0, 0, w, h);
        THIS_WIN_PTR->_computeSquareRatio();
        THIS_WIN_PTR->_computeBoardSize();
    };
    glfwSetFramebufferSizeCallback(_win.win, framebuffer_size_callback);
}

void
GlfwGraphic::_computeSquareRatio()
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
GlfwGraphic::_computeBoardSize()
{
    _board.gl_board_size = _win.screen_ratio * (1.0f - VERTICAL_BORDER);

    float largest = (_board.w >= _board.h) ? _board.w : _board.h;
    _board.gl_snake_board_size = _board.gl_board_size / largest;

    _board.gl_board_size =
      glm::vec2(_board.w, _board.h) * _board.gl_snake_board_size;
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