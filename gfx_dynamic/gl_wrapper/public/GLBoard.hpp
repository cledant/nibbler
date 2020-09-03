#ifndef GLBOARD_HPP
#define GLBOARD_HPP

#include <stdexcept>

#include "glad/glad.h"
#include "glm/glm.hpp"

class GLBoard
{
  public:
    GLBoard();
    virtual ~GLBoard() = default;
    GLBoard(GLBoard const &src) = delete;
    GLBoard &operator=(GLBoard const &rhs) = delete;
    GLBoard(GLBoard &&src) noexcept;
    GLBoard &operator=(GLBoard &&rhs) noexcept;

    [[nodiscard]] uint32_t getVao() const;

    void init();
    void clear();

  private:
    static constexpr glm::vec3 const BOARD_COLOR = glm::vec3(0.0f);
    static constexpr glm::vec2 const BOARD_POS = glm::vec2(0.0f);

    [[nodiscard]] uint32_t _allocate_vbo(uint64_t type_size, void const *data) const;
    uint32_t _set_vao();

    uint8_t _is_init;
    uint32_t _vbo_pos;
    uint32_t _vbo_color;
    uint32_t _vao;
};

#endif