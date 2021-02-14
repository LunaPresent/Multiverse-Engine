#include "pch.h"
#include "Shader.h"

#include <glad/glad.h>

mv::Shader::Shader(const char* vertex_source, const char* fragment_source)
{
    // from https://learnopengl.com/Getting-started/Shaders

    int success;
    char infoLog[512];

    // vertex Shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    // print compile errors if any
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // fragment Shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);
    // print compile errors if any
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // shader Program
    this->_program_id = glCreateProgram();
    glAttachShader(this->_program_id, vertex_shader);
    glAttachShader(this->_program_id, fragment_shader);
    glLinkProgram(this->_program_id);
    // print linking errors if any
    glGetProgramiv(this->_program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->_program_id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}
