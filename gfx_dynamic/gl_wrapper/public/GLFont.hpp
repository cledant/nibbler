#ifndef GLFONT_HPP
#define GLFONT_HPP

#include <map>
#include <string>

#include "glad/glad.h"
#include "ft2build.h"
#include "glm/glm.hpp"

#include "GLShader.hpp"

#include FT_FREETYPE_H

class GLFont
{
  public:
    GLFont();
    virtual ~GLFont() = default;
    GLFont(GLFont const &src) = delete;
    GLFont &operator=(GLFont const &rhs) = delete;
    GLFont(GLFont &&src) noexcept;
    GLFont &operator=(GLFont &&rhs) noexcept;

    void init(std::string const &path_font,
              std::string const &path_vs,
              std::string const &path_fs,
              glm::vec2 const &window_size,
              uint64_t font_size);
    void clear();

    void setOrthographicProjection(glm::vec2 const &window_size);

    void drawText(std::string const &str,
                  glm::vec3 const &color,
                  glm::vec2 const &pos,
                  float scale);

  private:
    struct FontChar
    {
        uint32_t tex;
        glm::ivec2 size;
        glm::ivec2 bearing;
        size_t advance;
    };

    void _loadFont(std::string const &path);
    void _allocate_vbo();
    void _allocate_vao();
    static uint32_t _create_glyph_texture(const void *buffer,
                                          uint32_t tex_w,
                                          uint32_t tex_h);

    uint8_t _is_init;
    uint64_t _font_size;
    GLShader _shader;
    glm::vec2 _win_size;
    glm::mat4 _ortho;
    std::map<char, FontChar> _char_list;
    uint32_t _vao;
    uint32_t _vbo;
};

#endif
