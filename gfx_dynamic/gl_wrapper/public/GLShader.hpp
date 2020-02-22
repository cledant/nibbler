#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>

#include "glad/glad.h"
#include "glm/glm.hpp"

class GLShader
{
  public:
    GLShader();
    virtual ~GLShader() = default;
    GLShader(GLShader &&src) noexcept;
    GLShader &operator=(GLShader &&rhs) noexcept;
    GLShader(GLShader const &src) = delete;
    GLShader &operator=(GLShader const &rhs) = delete;

    void init(std::string const &path_vs,
              std::string const &path_fs,
              std::string const &prog_name);
    void init(std::string const &path_vs,
              std::string const &path_gs,
              std::string const &path_fs,
              std::string const &prog_name);
    void clear();
    void use() const;
    void setVec2(std::string const &name, glm::vec2 const &data);
    void setVec3(std::string const &name, glm::vec3 const &data);
    void setMat4(std::string const &name, glm::mat4 const &data);
    void setInt(std::string const &name, int data);

  private:
    void _readFile(std::string const &path, std::string &content) const;
    [[nodiscard]] uint32_t _loadShader(std::string const &path,
                                       int32_t shader_type) const;
    [[nodiscard]] uint32_t _linkShaders(int32_t vs, int32_t fs) const;
    [[nodiscard]] uint32_t _linkShaders(int32_t vs,
                                        int32_t gs,
                                        int32_t fs) const;
    [[nodiscard]] std::string _shaderError(uint32_t shader) const;
    void _setUniform(std::string const &name);

    uint8_t _is_init;
    uint32_t _program;
    std::map<std::string, int32_t> _uniform_id;
};

#endif