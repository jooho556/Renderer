#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Shader.h"

namespace
{
    constexpr unsigned int LOGSIZE = 8096;
}

//////////
// ShaderBase Implementation
//////////

void ShaderBase::Use() const { glUseProgram(m_program_id); }

//Uniform setting
void ShaderBase::SetFloat(const std::string & name, float value) const {
    glUniform1f(CheckUniformLocation(name), value);
}

void ShaderBase::SetInt(const std::string & name, int value) const {
    glUniform1i(CheckUniformLocation(name), value);
}

void ShaderBase::SetBool(const std::string & name, bool value) const {
    glUniform1i(CheckUniformLocation(name), static_cast<int>(value));
}

void ShaderBase::SetMat4(const std::string & name, const glm::mat4 & mat) const {
    glUniformMatrix4fv(CheckUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderBase::SetVec3(const std::string & name, const glm::vec3 & vec) const {
    glUniform3fv(CheckUniformLocation(name), 1, &vec[0]);
}

void ShaderBase::SetVec2(const std::string& name, const glm::vec2& vec) const {
    glUniform2fv(CheckUniformLocation(name), 1, &vec[0]);
}

ShaderBase::~ShaderBase() {
    if (!is_copied) glDeleteProgram(m_program_id);
}

unsigned int ShaderBase::CheckUniformLocation(const std::string & name) const
{
    unsigned int location = glGetUniformLocation(m_program_id, name.c_str());
    if (location == -1)
    {
        std::cout << "ERROR: CAN'T FIND UNIFORM LOCATION: " << name << std::endl;
        assert(0);
    }    
    return location;
}

void ShaderBase::CheckProgramLink(unsigned int program_id) const
{
    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[LOGSIZE];
        glGetProgramInfoLog(program_id, LOGSIZE, NULL, log);
        std::cout << "ERROR: PROGRAM LINK FAILED:\n" << log << std::endl;
        assert(0);
    }
}

unsigned int ShaderBase::CreateShader(const std::string & source_path, unsigned int shader_type)
{
    unsigned int shader = glCreateShader(shader_type);

    //Read shader source
    std::string source;
    std::ifstream infile(source_path);
    if (infile.is_open())
    {
        std::stringstream ss;
        ss << infile.rdbuf(); // overloded << operator handling stream buffer object
        source = ss.str();
        infile.close();
    }

    const char * psource = source.c_str();
    glShaderSource(shader, 1, &psource, NULL);

    return shader;
}

void ShaderBase::CompileShader(unsigned int shader)
{
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[LOGSIZE];
        glGetShaderInfoLog(shader, LOGSIZE, NULL, log);
        std::cout << "ERROR: SHADER COMPILATION FAILED:\n" << log << std::endl;
        assert(0);
    }
}


//////////
// Pipeline Shader Implementation
//////////

Shader::Shader(const std::string & vertex_path, const std::string & fragment_path)
{
    unsigned int vertex = CreateShader(vertex_path, GL_VERTEX_SHADER);
    unsigned int fragment = CreateShader(fragment_path, GL_FRAGMENT_SHADER);
    CompileShader(vertex);
    CompileShader(fragment);
    CreateProgram(vertex, fragment);
}

void Shader::CreateProgram(unsigned int vertex, unsigned int fragment)
{
    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, vertex);
    glAttachShader(m_program_id, fragment);
    glLinkProgram(m_program_id);

    CheckProgramLink(m_program_id);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


//////////
// Compute Shader Implementation
//////////

ComputeShader::ComputeShader(const std::string & compute_path)
{
    unsigned int compute = CreateShader(compute_path, GL_COMPUTE_SHADER);
    CompileShader(compute);
    CreateProgram(compute);
}

void ComputeShader::CreateProgram(unsigned int compute)
{
    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, compute);
    glLinkProgram(m_program_id);

    CheckProgramLink(m_program_id);

    glDeleteShader(compute);
}