#include <iostream>
#include <fstream>

#include "GLShader.hpp"

GLShader::GLShader()
  : _is_init(0)
  , _program(0)
{}

GLShader::GLShader(GLShader &&src) noexcept
{
    _is_init = src._is_init;
    _program = src._program;
    _uniform_id = std::move(src._uniform_id);
    src._program = 0;
}

GLShader &
GLShader::operator=(GLShader &&rhs) noexcept
{
    _is_init = rhs._is_init;
    _program = rhs._program;
    _uniform_id = std::move(rhs._uniform_id);
    rhs._program = 0;
    return (*this);
}

void
GLShader::init(std::string const &path_vs,
             std::string const &path_fs,
             std::string const &prog_name)
{
    if (_is_init) {
        return;
    }
    uint32_t vs = 0;
    uint32_t fs = 0;
    std::cout << "Loading Shader : " << prog_name << std::endl;

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
    _is_init = 1;
}

void
GLShader::init(std::string const &path_vs,
             std::string const &path_gs,
             std::string const &path_fs,
             std::string const &prog_name)
{
    if (_is_init) {
        return;
    }
    uint32_t vs = 0;
    uint32_t gs = 0;
    uint32_t fs = 0;
    std::cout << "Loading Shader : " << prog_name << std::endl;

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
    _is_init = 1;
}

void
GLShader::clear()
{
    if (_program) {
        glDeleteShader(_program);
    }
    _program = 0;
    _uniform_id.clear();
    _is_init = 0;
}

void
GLShader::use() const
{
    glUseProgram(_program);
}

void
GLShader::setVec2(std::string const &name, glm::vec2 const &data)
{
    auto entry = _uniform_id.find(name);
    if (entry == _uniform_id.end()) {
        int32_t id = glGetUniformLocation(_program, name.c_str());
        if (id < 0) {
            throw std::runtime_error("GLShader: Invalid uniforn name: " + name);
        }
        _uniform_id[name] = id;
    }
    glUniform2fv(
      _uniform_id[name], 1, reinterpret_cast<GLfloat const *>(&data));
}

void
GLShader::_readFile(std::string const &path, std::string &content) const
{
    try {
        std::fstream fs;
        fs.exceptions(std::fstream::failbit | std::fstream::badbit);
        fs.open(path, std::fstream::in);
        content.assign((std::istreambuf_iterator<char>(fs)),
                       std::istreambuf_iterator<char>());
        fs.close();
    } catch (std::exception &e) {
        throw std::runtime_error("GLShader: Failed to open: " + path);
    }
}

uint32_t
GLShader::_loadShader(std::string const &path, int32_t shader_type) const
{
    std::string shader_code;
    _readFile(path, shader_code);
    char const *c_shader_code = shader_code.c_str();

    int32_t success;
    uint32_t shader = glCreateShader(shader_type);
    if (!shader) {
        throw std::runtime_error("GLShader: Failed to allocate shader: " + path);
    }
    glShaderSource(shader, 1, &c_shader_code, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        throw std::runtime_error("GLShader: Failed to compile shader: " + path +
                                 "\n" + _shaderError(shader));
    }
    return (shader);
}

uint32_t
GLShader::_linkShaders(int32_t vs, int32_t fs) const
{
    uint32_t program = glCreateProgram();

    if (!program) {
        throw std::runtime_error("GLShader: Failed to allocate shader program");
    }
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteShader(program);
        throw std::runtime_error("GLShader: Failed to link shader program");
    }
    return (program);
}

uint32_t
GLShader::_linkShaders(int32_t vs, int32_t gs, int32_t fs) const
{
    uint32_t program = glCreateProgram();

    if (!program) {
        throw std::runtime_error("GLShader: Failed to allocate shader program");
    }
    glAttachShader(program, vs);
    glAttachShader(program, gs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int32_t success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteShader(program);
        throw std::runtime_error("GLShader: Failed to link shader program");
    }
    return (program);
}

std::string
GLShader::_shaderError(uint32_t shader) const
{
    char msg[4096];
    int msg_len;

    glGetShaderInfoLog(shader, 4095, &msg_len, msg);
    msg[msg_len] = '\0';
    return (std::string(msg));
}