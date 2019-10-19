#include "Snake.hpp"

#include <cstring>

Snake::Snake()
  : _head_color(1.0f)
  , _body_color(_head_color / 2.0f)
  , _snake_pos()
  , _snake_color()
  , _cur_size(0)
  , _max_size(0)
  , _board_w(0)
  , _board_h(0)
{}

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
    return (_snake_pos[_cur_size]);
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

#include <iostream>

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