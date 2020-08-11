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

void ShaderBase::Use() const { glUseProgram(m_id); }

//Uniform setting
void ShaderBase::SetFloat(const std::string & name, float value) {
    glUniform1f(CheckUniformLocation(name), value);
}

void ShaderBase::SetInt(const std::string & name, int value) {
    glUniform1i(CheckUniformLocation(name), value);
}

void ShaderBase::SetBool(const std::string & name, bool value) {
    glUniform1i(CheckUniformLocation(name), static_cast<int>(value));
}

void ShaderBase::SetMat4(const std::string & name, const glm::mat4 & mat) {
    glUniformMatrix4fv(CheckUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderBase::SetVec3(const std::string & name, const glm::vec3 & vec) {
    glUniform3fv(CheckUniformLocation(name), 1, &vec[0]);
}

void ShaderBase::SetVec2(const std::string& name, const glm::vec2& vec) {
    glUniform2fv(CheckUniformLocation(name), 1, &vec[0]);
}

ShaderBase::~ShaderBase() {
    Destroy();
}

void ShaderBase::Destroy()
{
    if (!m_is_copied)
    {
        glUseProgram(0);
        glDeleteProgram(m_id);
        m_id = 0;
    }
}

unsigned int ShaderBase::CheckUniformLocation(const std::string & name)
{
    unsigned int location = glGetUniformLocation(m_id, name.c_str());
    if (location == -1)
    {
        error_log = "ERROR: CAN'T FIND UNIFORM LOCATION: " + name;        
        //assert(0);
    }    
    return location;
}

void ShaderBase::CheckProgramLink(unsigned int program_id)
{
    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        int log_length;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        std::string str;
        str.reserve(log_length);
        glGetProgramInfoLog(program_id, log_length, NULL, &str[0]);
        error_log += "ERROR: PROGRAM LINK FAILED:\n";
        error_log += str + "\n\n";

        //std::cout << "ERROR: PROGRAM LINK FAILED:\n" << log << std::endl;
        //assert(0);

        return;
    }
    if (!error_log.empty())
        error_log.clear();
}

unsigned int ShaderBase::CreateShader(const std::string & source_path, unsigned int shader_type)
{
    unsigned int shader = glCreateShader(shader_type);

    //Read shader source
    std::ifstream infile(source_path);
    std::string source;
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
        int log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        std::string str(log_length, '\0');
        glGetShaderInfoLog(shader, log_length, NULL, &str[0]);

        error_log += "ERROR: SHADER COMPILATION FAILED:\n";
        error_log += str;

        //std::cout << "ERROR: SHADER COMPILATION FAILED:\n" << error_log << std::endl;
        //assert(0);
    }
}


//////////
// Pipeline Shader Implementation
//////////

Shader::Shader(const std::string & vertex_path, const std::string & fragment_path)    
    : vert_path(vertex_path), frag_path(fragment_path)
{
    Construct(vertex_path, fragment_path);
}

void Shader::Compile()
{
    Destroy();
    Construct(vert_path, frag_path);
}

void Shader::Construct(const std::string& vertex_path, const std::string& fragment_path)
{
    unsigned int vertex = CreateShader(vertex_path, GL_VERTEX_SHADER);
    unsigned int fragment = CreateShader(fragment_path, GL_FRAGMENT_SHADER);
    CompileShader(vertex);
    CompileShader(fragment);
    CreateProgram(vertex, fragment);
}

void Shader::CreateProgram(unsigned int vertex, unsigned int fragment)
{
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);

    CheckProgramLink(m_id);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


//////////
// Compute Shader Implementation
//////////

ComputeShader::ComputeShader(const std::string & compute_path)
    : comp_path(compute_path)
{  
    Construct(compute_path);
}

void ComputeShader::Compile()
{
    Destroy();
    Construct(comp_path);
}

void ComputeShader::Compute(unsigned int groupnum_x, unsigned int groupnum_y,
    unsigned int groupnum_z, int barrier_type)
{
    glDispatchCompute(groupnum_x, groupnum_y, groupnum_z);
    glMemoryBarrier(barrier_type);
}

void ComputeShader::Construct(const std::string& compute_path)
{
    unsigned int compute = CreateShader(compute_path, GL_COMPUTE_SHADER);
    CompileShader(compute);
    CreateProgram(compute);
}

void ComputeShader::CreateProgram(unsigned int compute)
{
    m_id = glCreateProgram();
    glAttachShader(m_id, compute);
    glLinkProgram(m_id);

    CheckProgramLink(m_id);

    glDeleteShader(compute);
}