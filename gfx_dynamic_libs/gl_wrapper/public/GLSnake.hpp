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
    void updateScreenPos(
      std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      void (*converter)(
        std::array<glm::uvec2, IGraphicConstants::MAX_SNAKE_SIZE> const &src,
        std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &dst),
      uint32_t size);
    void updateVbo(
      std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> const &pos,
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size);
    void updateVbo(
      std::array<glm::vec3, IGraphicConstants::MAX_SNAKE_SIZE> const &color,
      uint32_t size);

  private:
    static uint32_t _allocate_vbo(uint64_t type_size, uint32_t size);
    uint32_t _set_vao();

    uint8_t _is_init;
    uint32_t _vbo_pos;
    uint32_t _vbo_color;
    uint32_t _vao;
    std::array<glm::vec2, IGraphicConstants::MAX_SNAKE_SIZE> _screen_pos;
    uint32_t _screen_pos_size;
};

#endif
