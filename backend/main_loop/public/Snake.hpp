#ifndef SQUAREARRAY_HPP
#define SQUAREARRAY_HPP

#include <cstdint>
#include <array>

#include "glm/glm.hpp"

#include "IGraphicConstants.hpp"

class Snake
{
  public:
    enum Direction
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

    [[nodiscard]] std::array<glm::uvec2,
                             IGraphicConstants::MAX_SNAKE_SIZE> const &
    getSnakePosArray() const;
    [[nodiscard]] std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const
      &
      getSnakeColorArray() const;
    [[nodiscard]] uint32_t getSnakeCurrentSize() const;
    [[nodiscard]] uint32_t getSnakeMaxSize() const;
    [[nodiscard]] glm::uvec2 const &getSnakeHeadPos() const;

    void init(glm::uvec2 const &start_pos,
              glm::vec3 base_color,
              uint32_t board_w,
              uint32_t board_h);
    void addToSnake(glm::uvec2 const &pos);
    void addToSnake(glm::uvec2 const &pos, glm::vec3 const &color);
    void removeFromSnake(glm::uvec2 const &pos);
    uint8_t isInsideSnake(glm::uvec2 const &pos);
    uint8_t moveSnake(enum Direction dir);

  private:
    static constexpr std::array<glm::ivec2, 4> _offset = { glm::ivec2{ 0, -1 },
                                                           glm::ivec2{ 0, 1 },
                                                           glm::ivec2{ 1, 0 },
                                                           glm::ivec2{ -1,
                                                                       0 } };

    glm::vec3 _head_color;
    glm::vec3 _body_color;
    std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> _snake_pos;
    std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> _snake_color;
    uint32_t _cur_size;
    uint32_t _max_size;
    int32_t _board_w;
    int32_t _board_h;
};

#endif
