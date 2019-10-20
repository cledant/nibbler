#include "Snake.hpp"

#include <cstring>

Snake::Snake()
  : _head_color(1.0f)
  , _body_color(_head_color / 2.0f)
  , _snake_pos()
  , _snake_color()
  , _cur_size(1)
  , _max_size(1)
  , _board_w(0)
  , _board_h(0)
{
    _snake_pos[0] = glm::uvec2(100);
    _snake_color[0] = glm::vec3(1.0f);
}

std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> const &
Snake::getSnakePosArray() const
{
    return (_snake_pos);
}

std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &
Snake::getSnakeColorArray() const
{
    return (_snake_color);
}

uint32_t
Snake::getSnakeCurrentSize() const
{
    return (_cur_size);
}

uint32_t
Snake::getSnakeMaxSize() const
{
    return (_max_size);
}

glm::uvec2 const &
Snake::getSnakeHeadPos() const
{
    return (_snake_pos[_cur_size - 1]);
}

void
Snake::init(glm::uvec2 const &start_pos,
            glm::vec3 base_color,
            uint32_t board_w,
            uint32_t board_h)
{
    _head_color = base_color;
    _body_color = _head_color / 2.0f;
    _board_w = board_w;
    _board_h = board_h;
    _max_size = _board_h * _board_w;
    _cur_size = 4;

    for (uint32_t i = 0; i < 4; ++i) {
        _snake_pos[i] = glm::uvec2{ start_pos.x, start_pos.y + (3 - i) };
        _snake_color[i] = _body_color;
    }
    _snake_color[3] = _head_color;
}

void
Snake::addToSnake(glm::uvec2 const &pos)
{
    if (_cur_size < _max_size) {
        _snake_pos[_cur_size] = pos;
        _snake_color[_cur_size - 1] = _body_color;
        _snake_color[_cur_size] = _head_color;
        ++_cur_size;
    }
}

void
Snake::addToSnake(glm::uvec2 const &pos, glm::vec3 const &color)
{
    if (_cur_size < _max_size) {
        _snake_pos[_cur_size] = pos;
        std::memmove(_snake_color.data() + 1,
                     _snake_color.data(),
                     sizeof(glm::uvec2) * (_cur_size - 1));
        _snake_color[_cur_size] = color;
        ++_cur_size;
    }
}

void
Snake::removeFromSnake(glm::uvec2 const &pos)
{
    if (_cur_size == 1) {
        _snake_pos[0] = glm::uvec2(100);
        return;
    }
    uint32_t i;
    for (i = 0; i < _cur_size; ++i) {
        if (_snake_pos[i] == pos) {
            break;
        }
    }
    if (i == _cur_size) {
        return;
    }
    std::memmove(_snake_pos.data() + i,
                 _snake_pos.data() + i + 1,
                 sizeof(glm::uvec2) * (_cur_size - 1 - i));
    std::memmove(_snake_color.data() + i,
                 _snake_color.data() + i + 1,
                 sizeof(glm::vec3) * (_cur_size - 1 - i));
    --_cur_size;
}

uint8_t
Snake::isInsideSnake(glm::uvec2 const &pos)
{
    for (auto const &it : _snake_pos) {
        if (it == pos) {
            return (1);
        }
    }
    return (0);
}

uint8_t
Snake::moveSnake(enum Direction dir)
{
    if (!_cur_size) {
        return (1);
    }
    glm::ivec2 updated_head_pos = _snake_pos[_cur_size - 1];
    updated_head_pos += _offset[dir];
    if (updated_head_pos.x < 0 || updated_head_pos.x >= _board_w ||
        updated_head_pos.y < 0 || updated_head_pos.y >= _board_h) {
        return (1);
    }
    std::memmove(_snake_pos.data(),
                 _snake_pos.data() + 1,
                 sizeof(glm::uvec2) * (_cur_size - 1));
    _snake_pos[_cur_size - 1] = updated_head_pos;
    return (0);
}