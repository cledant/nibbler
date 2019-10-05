#ifndef GLSNAKE_HPP
#define GLSNAKE_HPP

#include <array>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "IGraphicConstants.hpp"

class GLSnake
{
  public:
    GLSnake();
    virtual ~GLSnake();
    GLSnake(GLSnake const &src) = delete;
    GLSnake &operator=(GLSnake const &rhs) = delete;
    GLSnake(GLSnake &&src);
    GLSnake &operator=(GLSnake &&rhs);

    [[nodiscard]] uint32_t getVao() const;
    void updateScreenPos(
      std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos);
    void updateVbo(
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size);
    void updateVbo(
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size);

  private:
    uint32_t vbo_pos;
    uint32_t vbo_color;
    uint32_t vao;
    std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> _screen_pos;
};

#endif
