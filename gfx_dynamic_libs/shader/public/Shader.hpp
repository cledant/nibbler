#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>

#include "glad/glad.h"
#include "glm/glm.hpp"

class Shader
{
  public:
    Shader(std::string const &path_vs,
           std::string const &path_fs,
           std::string const &prog_name);
    Shader(std::string const &path_vs,
           std::string const &path_gs,
           std::string const &path_fs,
           std::string const &prog_name);
    virtual ~Shader();
    Shader(Shader &&src) noexcept;
    Shader &operator=(Shader &&rhs) noexcept;
    Shader(Shader const &src) = delete;
    Shader &operator=(Shader const &rhs) = delete;

    void use() const;
    void setVec2(std::string const &name, glm::vec2 data);

  private:
    void _readFile(std::string const &path, std::string content) const;
    [[nodiscard]] uint32_t _loadShader(std::string const &path,
                                       int32_t shader_type) const;
    [[nodiscard]] uint32_t _linkShaders(int32_t vs, int32_t fs) const;
    [[nodiscard]] uint32_t _linkShaders(int32_t vs,
                                        int32_t gs,
                                        int32_t fs) const;

    uint32_t _program;
    std::map<std::string, int32_t> _uniform_id;
};

#endif