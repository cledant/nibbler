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
    virtual ~GLSnake() = default;
    GLSnake(GLSnake const &src) = delete;
    GLSnake &operator=(GLSnake const &rhs) = delete;
    GLSnake(GLSnake &&src) noexcept;
    GLSnake &operator=(GLSnake &&rhs) noexcept;

    [[nodiscard]] uint32_t getVao() const;

    void init();
    void clear();
    void updateVbo(
      std::array<glm::ivec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      glm::vec2 const &square_size,
      int32_t board_w,
      int32_t board_h,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size);
    void updateVbo(
      std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size);

  private:
    [[nodiscard]] uint32_t _allocate_vbo(uint64_t type_size,
                                         uint32_t size) const;
    uint32_t _set_vao();

    uint8_t _is_init;
    uint32_t _vbo_pos;
    uint32_t _vbo_color;
    uint32_t _vao;
    std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> _screen_pos;
};

#endif
