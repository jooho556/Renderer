#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

Shader::Shader(const std::string & vertex_path, const std::string & fragment_path)
{
    unsigned int vertex = CreateShader(vertex_path, ShaderType::Vertex);
    unsigned int fragment = CreateShader(fragment_path, ShaderType::Fragment);
    CompileShader(vertex);
    CompileShader(fragment);
    CreateProgram(vertex, fragment);
}

Shader::~Shader()
{
    glDeleteProgram(m_programID);
}

void Shader::Use() const
{
    glUseProgram(m_programID);
}

void Shader::SetFloat(const std::string & name, float value) const
{
    int location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1)
        glUniform1f(location, value);
}

void Shader::SetInt(const std::string & name, int value) const
{
    int location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1)
        glUniform1i(location, value);
}

void Shader::SetBoll(const std::string & name, bool value) const
{
    int location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1)
        glUniform1i(location, static_cast<int>(value));
}

void Shader::SetMat4(const std::string & name, const glm::mat4 & mat) const
{
    unsigned int location = glGetUniformLocation(m_programID, name.c_str());
    if (location != -1)
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetVec3(const std::string & name, const glm::vec3 & vec) const
{
    unsigned int location = glGetUniformLocation(m_programID, name.c_str());
    if(location != -1)
        glUniform3fv(location, 1, &vec[0]);
}

unsigned int Shader::CreateShader(const std::string & source_path, ShaderType type)
{
    //Shader Creation
    unsigned int shader = 0;
    switch (type)
    {
    case ShaderType::Vertex:
    {
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
    }
    case ShaderType::Fragment:
    {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    }
    }

    //Source reading from a file
    std::string source;
    std::ifstream in_file(source_path);
    if (in_file.is_open())
    {
        std::stringstream sstream;
        sstream << in_file.rdbuf();
        source = sstream.str();
        in_file.close();
    }

    const char * p_source = source.c_str();
    glShaderSource(shader, 1, &p_source, NULL);

    return shader;
}

void Shader::CompileShader(unsigned int shader)
{
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cout << "ERROR: SHADER COMPILATION FAILED:\n" << log << std::endl;
    }
}

void Shader::CreateProgram(unsigned int vertex, unsigned int fragment)
{
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    glLinkProgram(m_programID);

    int success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(m_programID, 512, NULL, log);
        std::cout << "ERROR: PROGRAM LINK FAILED:\n" << log << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
