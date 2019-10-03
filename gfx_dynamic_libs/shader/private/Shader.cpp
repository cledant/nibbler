#include <iostream>
#include <fstream>

#include "Shader.hpp"

Shader::Shader(std::string const &path_vs,
               std::string const &path_fs,
               std::string const &prog_name)
  : _program(0)
{
    uint32_t vs = 0;
    uint32_t fs = 0;
    std::cout << "Loading : " << prog_name << std::endl;

    try {
        vs = _loadShader(path_vs, GL_VERTEX_SHADER);
        fs = _loadShader(path_fs, GL_FRAGMENT_SHADER);
        _program = _linkShaders(vs, fs);
    } catch (std::exception &e) {
        if (vs) {
            glDeleteShader(vs);
        }
        if (fs) {
            glDeleteShader(fs);
        }
        throw;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::Shader(std::string const &path_vs,
               std::string const &path_gs,
               std::string const &path_fs,
               std::string const &prog_name)
  : _program(0)
{
    uint32_t vs = 0;
    uint32_t gs = 0;
    uint32_t fs = 0;
    std::cout << "Loading : " << prog_name << std::endl;

    try {
        vs = _loadShader(path_vs, GL_VERTEX_SHADER);
        gs = _loadShader(path_gs, GL_GEOMETRY_SHADER);
        fs = _loadShader(path_fs, GL_FRAGMENT_SHADER);
        _program = _linkShaders(vs, gs, fs);
    } catch (std::exception &e) {
        if (vs) {
            glDeleteShader(vs);
        }
        if (gs) {
            glDeleteShader(gs);
        }
        if (fs) {
            glDeleteShader(fs);
        }
        throw;
    }
    glDeleteShader(vs);
    glDeleteShader(gs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    if (_program) {
        glDeleteShader(_program);
    }
}

Shader::Shader(Shader &&src) noexcept
{
    _program = src._program;
    _uniform_id = std::move(src._uniform_id);
    src._program = 0;
}

Shader &
Shader::operator=(Shader &&rhs) noexcept
{
    _program = rhs._program;
    _uniform_id = std::move(rhs._uniform_id);
    rhs._program = 0;
    return (*this);
}

void
Shader::use() const
{
    glUseProgram(_program);
}

void
Shader::setVec2(std::string const &name, glm::vec2 const &data)
{
    auto entry = _uniform_id.find(name);
    if (entry == _uniform_id.end()) {
        int32_t id = glGetUniformLocation(_program, name.c_str());
        if (id < 0) {
            throw std::runtime_error("Shader: Invalid uniforn name: " + name);
        }
        _uniform_id[name] = id;
    }
    glUniform2fv(
      _uniform_id[name], 1, reinterpret_cast<GLfloat const *>(&data));
}

void
Shader::_readFile(std::string const &path, std::string &content) const
{
    try {
        std::fstream fs;
        fs.exceptions(std::fstream::failbit | std::fstream::badbit);
        fs.open(path, std::fstream::in);
        content.assign((std::istreambuf_iterator<char>(fs)),
                       std::istreambuf_iterator<char>());
        fs.close();
    } catch (std::exception &e) {
        throw std::runtime_error("Shader: Failed to open: " + path);
    }
}

uint32_t
Shader::_loadShader(std::string const &path, int32_t shader_type) const
{
    std::string shader_code;
    _readFile(path, shader_code);
    char const *c_shader_code = shader_code.c_str();

    int32_t success;
    uint32_t shader = glCreateShader(shader_type);
    if (!shader) {
        throw std::runtime_error("Shader: Failed to allocate shader: " + path);
    }
    glShaderSource(shader, 1, &c_shader_code, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("Shader: Failed to compile shader: " + path +
                                 "\n" + _shaderError(shader));
    }
    return (shader);
}

uint32_t
Shader::_linkShaders(int32_t vs, int32_t fs) const
{
    uint32_t program = glCreateProgram();

    if (!program) {
        throw std::runtime_error("Shader: Failed to allocate shader program");
    }
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        throw std::runtime_error("Shader: Failed to link shader program");
    }
    return (program);
}

uint32_t
Shader::_linkShaders(int32_t vs, int32_t gs, int32_t fs) const
{
    uint32_t program = glCreateProgram();

    if (!program) {
        throw std::runtime_error("Shader: Failed to allocate shader program");
    }
    glAttachShader(program, vs);
    glAttachShader(program, gs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        throw std::runtime_error("Shader: Failed to link shader program");
    }
    return (program);
}

std::string
Shader::_shaderError(uint32_t shader) const
{
    char msg[4096];
    int msg_len;

    glGetShaderInfoLog(shader, 4095, &msg_len, msg);
    msg[msg_len] = '\0';
    return (std::string(msg));
}