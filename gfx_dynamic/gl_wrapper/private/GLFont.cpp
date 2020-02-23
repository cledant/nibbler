#include "GLFont.hpp"

#include <glm/gtc/matrix_transform.hpp>

GLFont::GLFont()
  : _is_init(0)
  , _font_size(1)
  , _shader()
  , _win_size(1.0f)
  , _ortho(1.0f)
  , _char_list()
  , _vao(0)
  , _vbo(0)
{}

GLFont::GLFont(GLFont &&src) noexcept
  : _is_init(0)
  , _font_size(1)
  , _shader()
  , _win_size(1.0f)
  , _ortho(1.0)
  , _char_list()
  , _vao(0)
  , _vbo(0)
{
    *this = std::move(src);
}

GLFont &
GLFont::operator=(GLFont &&rhs) noexcept
{
    _is_init = rhs._is_init;
    _shader = std::move(rhs._shader);
    _ortho = rhs._ortho;
    _char_list = rhs._char_list;
    _vao = rhs._vao;
    _vbo = rhs._vbo;
    rhs._vao = 0;
    rhs._vbo = 0;
    rhs._char_list.clear();
    rhs._is_init = 0;
    return (*this);
}

void
GLFont::init(std::string const &path_font,
             std::string const &path_vs,
             std::string const &path_fs,
             glm::vec2 const &window_size,
             uint64_t font_size)
{
    if (!_is_init) {
        _allocate_vbo();
        _allocate_vao();
        _font_size = font_size;
        _shader.init(path_vs, path_fs, "FontShader");
        _loadFont(path_font);
        _win_size = window_size;
        _ortho = glm::ortho(0.0f, _win_size.x, 0.0f, _win_size.y);
        _is_init = 1;
    }
}

void
GLFont::clear()
{
    _is_init = 0;
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    for (auto &it : _char_list) {
        glDeleteTextures(1, &it.second.tex);
    }
    _char_list.clear();
    _shader.clear();
}

void
GLFont::setOrthographicProjection(glm::vec2 const &window_size)
{
    _win_size = window_size;
    _ortho = glm::ortho(0.0f, _win_size.x, 0.0f, _win_size.y);
}

void
GLFont::drawText(std::string const &str,
                 glm::vec3 const &color,
                 glm::vec2 const &pos,
                 float scale)
{
    float pos_x_offseted = pos.x;
    float pos_y = _win_size.y - pos.y;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    _shader.use();
    _shader.setMat4("uniform_mat_ortho", _ortho);
    _shader.setVec3("uniform_color", color);
    for (auto const &it : str) {
        auto fchar = _char_list.find(it);
        if (fchar == _char_list.end()) {
            fchar = _char_list.find('?');
        }

        float xpos = pos_x_offseted + fchar->second.bearing.x * scale;
        float ypos =
          pos_y - (fchar->second.size.y - fchar->second.bearing.y) * scale;
        float w = fchar->second.size.x * scale;
        float h = fchar->second.size.y * scale;
        float vertices[6][4] = { { xpos, ypos + h, 0.0, 0.0 },
                                 { xpos, ypos, 0.0, 1.0 },
                                 { xpos + w, ypos, 1.0, 1.0 },

                                 { xpos, ypos + h, 0.0, 0.0 },
                                 { xpos + w, ypos, 1.0, 1.0 },
                                 { xpos + w, ypos + h, 1.0, 0.0 } };
        glActiveTexture(GL_TEXTURE0);
        _shader.setInt("uniform_tex", 0);
        glBindTexture(GL_TEXTURE_2D, fchar->second.tex);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 6 * 4, vertices);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        pos_x_offseted += (fchar->second.advance >> 6) * scale;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void
GLFont::_loadFont(std::string const &path)
{
    FT_Library lib;
    FT_Face face;

    if (FT_Init_FreeType(&lib)) {
        throw std::runtime_error("GLFont: Freetype init failed");
    }
    if (FT_New_Face(lib, path.c_str(), 0, &face)) {
        FT_Done_FreeType(lib);
        throw std::runtime_error("GLFont: Failed to load font");
    }
    FT_Set_Pixel_Sizes(face, 0, _font_size);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable texture alignment
    for (uint8_t i = 0; i < 128; ++i) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            FT_Done_Face(face);
            FT_Done_FreeType(lib);
            throw std::runtime_error("GLFont: Failed to load glyph");
        }
        try {
            FontChar fchar = {
                _create_glyph_texture(face->glyph->bitmap.buffer,
                                      face->glyph->bitmap.width,
                                      face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<uint32_t>(face->glyph->advance.x)
            };
            auto ret =
              this->_char_list.emplace(std::pair<char, FontChar>(i, fchar));
            if (!ret.second) {
                throw std::runtime_error("GLFont: Failed to insert glyph");
            }
        } catch (std::exception &e) {
            FT_Done_Face(face);
            FT_Done_FreeType(lib);
            throw;
        }
    }
    FT_Done_Face(face);
    FT_Done_FreeType(lib);
}

void
GLFont::_allocate_vbo()
{
    glGenBuffers(1, &_vbo);
    if (!_vbo) {
        throw std::runtime_error("GLFont: Failed to create buffer");
    }
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    if (glGetError() == GL_OUT_OF_MEMORY) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        throw std::runtime_error("GLFont: Failed to allocate buffer");
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
GLFont::_allocate_vao()
{
    glGenVertexArrays(1, &_vao);
    if (!_vao) {
        throw std::runtime_error("GLFont: Failed to create vao");
    }
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexAttribPointer(
      0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

uint32_t
GLFont::_create_glyph_texture(const void *buffer,
                              uint32_t tex_w,
                              uint32_t tex_h)
{
    uint32_t tex_id;

    glGenTextures(1, &tex_id);
    if (!tex_id) {
        throw std::runtime_error("GLFont: Failed to create texture for glyph");
    }
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 tex_w,
                 tex_h,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    return (tex_id);
}