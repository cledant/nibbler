#include "GLBoard.hpp"

GLBoard::GLBoard()
  : _is_init(0)
  , _vbo_pos(0)
  , _vbo_color(0)
  , _vao(0)
{}

GLBoard::GLBoard(GLBoard &&src) noexcept
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

GLBoard &
GLBoard::operator=(GLBoard &&rhs) noexcept
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
GLBoard::init()
{
    if (_is_init) {
        return;
    }
    try {
        _vbo_pos = _allocate_vbo(sizeof(glm::vec2), &BOARD_POS);
        _vbo_color = _allocate_vbo(sizeof(glm::vec3), &BOARD_COLOR);
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
GLBoard::clear()
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
GLBoard::getVao() const
{
    return (_vao);
}

uint32_t
GLBoard::_allocate_vbo(uint64_t type_size, void const *data) const
{
    uint32_t vbo;

    glGenBuffers(1, &vbo);
    if (!vbo) {
        throw std::runtime_error("GLBoard: Failed to create buffer");
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, type_size, data, GL_STATIC_DRAW);
    if (glGetError() == GL_OUT_OF_MEMORY) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw std::runtime_error("GLBoard: Failed to allocate buffer");
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return (vbo);
}

uint32_t
GLBoard::_set_vao()
{
    uint32_t vao;

    glGenVertexArrays(1, &vao);
    if (!vao) {
        throw std::runtime_error("GLBoard: Failed to create vao");
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