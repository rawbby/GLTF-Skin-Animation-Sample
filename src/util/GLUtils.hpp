#pragma once

#include <cstdlib>
#include <cstdint>
#include <memory>
#include <fstream>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

#include <util/Assert.hpp>

/*
std::string load_text (std::string_view path)
{
    std::fstream ifs{path.data(), std::ios::in | std::ios::binary | std::ios::ate};
    ASSERT(ifs, "Could not open file! {}", path.data());

    const auto size = static_cast<size_t> (ifs.tellg());

    ifs.seekg(0, std::ios::beg);

    std::string buffer{};
    buffer.resize(size);

    ifs.read(buffer.data(), size);
    return buffer;
}
*/

GLuint compile_shader (GLuint type, std::string_view shader_source)
{
    GLuint shader = glCreateShader(type);

    const auto shader_data = shader_source.data();
    glShaderSource(shader, 1, &shader_data, nullptr);

    glCompileShader(shader);

    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

    if (GL_TRUE != compile_status)
    {
        GLint info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

        auto info_log = std::vector<GLchar>(info_log_length);
        glGetShaderInfoLog(shader, info_log_length, &info_log_length, info_log.data());

        spdlog::error("OPENGL SHADER COMPILATION FAILED! (FILE: \"{}\", LINE: \"{}\")\n{}\n{}", __FILE__, __LINE__, info_log.data(), shader_source);

        glDeleteShader(shader);
        return GL_NONE;
    }

    return shader;
}

GLuint compile_program (GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (GL_TRUE != link_status)
    {
        GLint info_log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

        auto info_log = std::vector<GLchar>(info_log_length);
        glGetProgramInfoLog(program, info_log_length, &info_log_length, info_log.data());

        spdlog::error("OPENGL PROGRAM LINK FAILED! (FILE: \"{}\", LINE: \"{}\")\n{}", __FILE__, __LINE__, info_log.data());

        glDeleteProgram(program);
        return GL_NONE;
    }

    return program;
}
