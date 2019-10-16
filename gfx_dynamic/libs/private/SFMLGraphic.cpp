#include <stdexcept>
#include <cstring>

#include "SFMLGraphic.hpp"

SFMLGraphic::SFMLGraphic()
  : _win()
  , _board()
  , _should_close(0)
{
    _win.fullscreen = 0;
    _win.w = 0;
    _win.h = 0;
    _win.w_viewport = 0;
    _win.h_viewport = 0;
    _win._screen_ratio = glm::vec2(1.0f);
    _board.h = 0;
    _board.w = 0;
    _board.gl_snake_board_size = glm::vec2(1.0f);
    _board.gl_board_size = glm::vec2(1.0f);
}

void
SFMLGraphic::init(std::string const &home, int32_t w_square, int32_t h_square)
{
    _board.w = w_square;
    _board.h = h_square;
    _home = home;
}

void
SFMLGraphic::terminate()
{}

void
SFMLGraphic::createWindow(std::string &&name)
{
    _win._win_name = name;
    _createWindow();
}

void
SFMLGraphic::deleteWindow()
{
    _gl_board.clear();
    _gl_snake.clear();
    _gl_snake_shader.clear();
    _win.win.close();
}

uint8_t
SFMLGraphic::shouldClose()
{
    return (_should_close);
}

void
SFMLGraphic::triggerClose()
{
    _should_close = 1;
}

void
SFMLGraphic::getEvents(std::array<uint8_t, IGraphicConstants::NB_EVENT> &events)
{
    auto buffer = events.data();

    if (!_win.win.hasFocus()) {
        std::memset(buffer, 0, sizeof(uint8_t) * IGraphicConstants::NB_EVENT);
        return;
    }
    buffer[IGraphicTypes::NibblerEvent::CLOSE_WIN] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
    buffer[IGraphicTypes::NibblerEvent::PAUSE] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    buffer[IGraphicTypes::NibblerEvent::TOGGLE_WIN] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::F5);
    buffer[IGraphicTypes::NibblerEvent::P1_UP] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    buffer[IGraphicTypes::NibblerEvent::P1_RIGHT] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    buffer[IGraphicTypes::NibblerEvent::P1_DOWN] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    buffer[IGraphicTypes::NibblerEvent::P1_LEFT] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    buffer[IGraphicTypes::NibblerEvent::P2_UP] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    buffer[IGraphicTypes::NibblerEvent::P2_RIGHT] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    buffer[IGraphicTypes::NibblerEvent::P2_DOWN] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    buffer[IGraphicTypes::NibblerEvent::P2_LEFT] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    buffer[IGraphicTypes::NibblerEvent::SET_GLFW] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::F1);
    buffer[IGraphicTypes::NibblerEvent::SET_SFML] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::F2);
    buffer[IGraphicTypes::NibblerEvent::SET_SDL] =
      sf::Keyboard::isKeyPressed(sf::Keyboard::F3);
}

void
SFMLGraphic::drawBoard()
{
    _gl_snake_shader.use();
    _gl_snake_shader.setVec2("uniform_scale", _board.gl_board_size);
    glBindVertexArray(_gl_board.getVao());
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
}

void
SFMLGraphic::drawSnake(
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
SFMLGraphic::drawSnake(
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
SFMLGraphic::render()
{
    _win.win.display();
}

void
SFMLGraphic::clear()
{
    glClearColor(0.321f, 0.254f, 0.361f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
SFMLGraphic::toggleFullscreen()
{
    _win.fullscreen = !_win.fullscreen;
    deleteWindow();
    _createWindow();
}

void
SFMLGraphic::_computeSquareRatio()
{
    float w =
      (_win.w_viewport >= _win.h_viewport) ? _win.w_viewport : _win.h_viewport;
    float h =
      (_win.w_viewport >= _win.h_viewport) ? _win.h_viewport : _win.w_viewport;

    _win._screen_ratio = (_win.w_viewport >= _win.h_viewport)
                           ? glm::vec2(h / w, 1.0f)
                           : glm::vec2(1.0f, h / w);
}

void
SFMLGraphic::_computeBoardSize()
{
    _board.gl_board_size = _win._screen_ratio * (1.0f - VERTICAL_BORDER);

    float largest = (_board.w >= _board.h) ? _board.w : _board.h;
    _board.gl_snake_board_size = _board.gl_board_size / largest;

    _board.gl_board_size =
      glm::vec2(_board.w, _board.h) * _board.gl_snake_board_size;
}

void
SFMLGraphic::_createWindow()
{
    sf::ContextSettings context;
    context.majorVersion = 4;
    context.minorVersion = 1;
    context.depthBits = 24;
    context.stencilBits = 8;
    if (_win.fullscreen) {
        _win.win.create(sf::VideoMode(),
                        _win._win_name,
                        sf::Style::Titlebar | sf::Style::Close |
                          sf::Style::Fullscreen,
                        context);
        _win.w = _win.win.getSize().x;
        _win.h = _win.win.getSize().y;
        _win.w_viewport = _win.w;
        _win.h_viewport = _win.h;
    } else {
        _win.win.create(
          sf::VideoMode(IGraphicConstants::WIN_W, IGraphicConstants::WIN_H),
          _win._win_name,
          sf::Style::Titlebar | sf::Style::Close,
          context);
        _win.w = IGraphicConstants::WIN_W;
        _win.h = IGraphicConstants::WIN_H;
        _win.w_viewport = IGraphicConstants::WIN_W;
        _win.h_viewport = IGraphicConstants::WIN_H;
        _win.win.setPosition({ 100, 100 });
    }
    _win.win.setVerticalSyncEnabled(false);
    _computeSquareRatio();
    _computeBoardSize();

    if (!gladLoadGL()) {
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
}

extern "C" IGraphic *
creator()
{
    return (new SFMLGraphic());
}

extern "C" void
deleter(IGraphic *gfx)
{
    delete gfx;
}