#ifndef SQUAREARRAY_HPP
#define SQUAREARRAY_HPP

#include <cstdint>
#include <array>

#include "glm/glm.hpp"

#include "IGraphicConstants.hpp"

class Snake
{
  public:
    enum snakeDirection
    {
        UP = 0,
        DOWN,
        RIGHT,
        LEFT
    };

    Snake();
    virtual ~Snake() = default;
    Snake(Snake const &src) = delete;
    Snake &operator=(Snake const &rhs) = delete;
    Snake(Snake &&src) = delete;
    Snake &operator=(Snake &&rhs) = delete;

    [[nodiscard]] std::array<glm::ivec2,
                             IGraphicConstants::MAX_SNAKE_SIZE> const &
    getSnakePosArray() const;
    [[nodiscard]] std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const
      &
      getSnakeColorArray() const;
    [[nodiscard]] uint32_t getSnakeCurrentSize() const;
    [[nodiscard]] uint32_t getSnakeMaxSize() const;
    [[nodiscard]] glm::ivec2 const &getSnakeHeadPos() const;
    [[nodiscard]] enum snakeDirection getSnakeDirection() const;

    void init(glm::ivec2 const &start_pos,
              glm::vec3 base_color,
              uint32_t board_w,
              uint32_t board_h);
    void setSnakeDirection(enum snakeDirection dir);
    void addToSnake(glm::ivec2 const &pos);
    void addToSnake(glm::ivec2 const &pos, glm::vec3 const &color);
    void removeFromSnake(glm::ivec2 const &pos);
    uint8_t isInsideSnake(glm::ivec2 const &pos);
    void moveSnake(enum snakeDirection dir);
    void moveSnakeWithCurrentDirection();

  private:
    static constexpr std::array<glm::ivec2, 4> _offset = { glm::ivec2{ 0, -1 },
                                                           glm::ivec2{ 0, 1 },
                                                           glm::ivec2{ 1, 0 },
                                                           glm::ivec2{ -1,
                                                                       0 } };

    enum snakeDirection _dir;
    glm::vec3 _head_color;
    glm::vec3 _body_color;
    std::array<glm::ivec2, IGraphicConstants::MAX_SNAKE_SIZE> _snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> _snake_color;
    uint32_t _cur_size;
    uint32_t _max_size;
    int32_t _board_w;
    int32_t _board_h;
};

#endif
