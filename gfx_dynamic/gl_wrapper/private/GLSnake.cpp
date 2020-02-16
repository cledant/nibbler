#include "GLSnake.hpp"

GLSnake::GLSnake()
  : _is_init(0)
  , _vbo_pos(0)
  , _vbo_color(0)
  , _vao(0)
  , _screen_pos()
{}

GLSnake::GLSnake(GLSnake &&src) noexcept
  : _screen_pos(src._screen_pos)
{
    _is_init = src._is_init;
    _vbo_color = src._vbo_color;
    _vbo_pos = src._vbo_pos;
    _vao = src._vao;
    src._vbo_pos = 0;
    src._vbo_color = 0;
    src._vao = 0;
    src._is_init = 0;
}

GLSnake &
GLSnake::operator=(GLSnake &&rhs) noexcept
{
    _is_init = rhs._is_init;
    _vbo_color = rhs._vbo_color;
    _vbo_pos = rhs._vbo_pos;
    _vao = rhs._vao;
    rhs._vbo_pos = 0;
    rhs._vbo_color = 0;
    rhs._vao = 0;
    rhs._is_init = 0;
    return (*this);
}

void
GLSnake::init()
{
    if (_is_init) {
        return;
    }
    try {
        _vbo_pos =
          _allocate_vbo(sizeof(glm::vec2), IGraphicConstants::MAX_SNAKE_SIZE);
        _vbo_color =
          _allocate_vbo(sizeof(glm::vec3), IGraphicConstants::MAX_SNAKE_SIZE);
        _vao = _set_vao();
    } catch (std::exception const &e) {
        if (_vbo_pos) {
            glDeleteBuffers(1, &_vbo_pos);
        }
        if (_vbo_color) {
            glDeleteBuffers(1, &_vbo_pos);
        }
        if (_vao) {
            glDeleteVertexArrays(1, &_vao);
        }
        throw;
    }
    _is_init = 1;
}

void
GLSnake::clear()
{
    if (_vbo_pos) {
        glDeleteBuffers(1, &_vbo_pos);
    }
    if (_vbo_color) {
        glDeleteBuffers(1, &_vbo_pos);
    }
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
    }
    _vbo_pos = 0;
    _vbo_color = 0;
    _vao = 0;
    _is_init = 0;
}

uint32_t
GLSnake::getVao() const
{
    return (_vao);
}

void
GLSnake::updateVbo(
  std::array<glm::ivec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
  glm::vec2 const &square_size,
  int32_t board_w,
  int32_t board_h,
  std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
  uint32_t size)
{
    glm::vec2 board_size = glm::vec2(board_w, board_h) * square_size;
    glm::vec2 init_pos = glm::vec2(-1.0f, 1.0f) * board_size;
    init_pos += glm::vec2(1.0f, -1.0f) * square_size;

    for (uint32_t i = 0; i < size; ++i) {
        _screen_pos[i] = init_pos + glm::vec2(1.0f, -1.0f) * square_size *
                                      2.0f * glm::vec2(pos[i].x, pos[i].y);
    }
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
    glBufferSubData(
      GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * size, _screen_pos.data());
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * size, color.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
GLSnake::updateVbo(
  std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
  std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
  uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * size, pos.data());
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * size, color.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint32_t
GLSnake::_allocate_vbo(uint64_t type_size, uint32_t size) const
{
    uint32_t vbo;

    glGenBuffers(1, &vbo);
    if (!vbo) {
        throw std::runtime_error("GLSnake: Failed to create buffer");
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, type_size * size, nullptr, GL_DYNAMIC_DRAW);
    if (glGetError() == GL_OUT_OF_MEMORY) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw std::runtime_error("GLSnake: Failed to allocate buffer");
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return (vbo);
}

uint32_t
GLSnake::_set_vao()
{
    uint32_t vao;

    glGenVertexArrays(1, &vao);
    if (!vao) {
        throw std::runtime_error("GLSnake: Failed to create vao");
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
    glVertexAttribPointer(
      0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_color);
    glVertexAttribPointer(
      1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return (vao);
}