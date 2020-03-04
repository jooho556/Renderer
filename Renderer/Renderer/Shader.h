#pragma once
#include <string>
#include <glm/glm.hpp>
#include "GLObject.h"

class ShaderBase : public GLObject
{
public:
    //Binding
    void Use() const;

    //Uniform setting
    void SetFloat(const std::string & name, float value) const;
    void SetInt(const std::string & name, int value) const;
    void SetBool(const std::string & name, bool value) const;
    void SetMat4(const std::string & name, const glm::mat4 & mat) const;
    void SetVec3(const std::string & name, const glm::vec3 & vec) const;
    void SetVec2(const std::string& name, const glm::vec2& vec) const;

protected:
    //Must be inherited!
    ShaderBase() {}
    ShaderBase(const ShaderBase & sb) : GLObject(sb) {}
    ShaderBase(ShaderBase && sb) : GLObject(sb) {}
    virtual ~ShaderBase() override;

protected:
    unsigned int CheckUniformLocation(const std::string & name) const;
    void CheckProgramLink(unsigned int program_id) const;
    unsigned int CreateShader(const std::string & source_path, unsigned int shader_type);
    void CompileShader(unsigned int shader);
};

class Shader : public ShaderBase //Pipeline shader
{
public:
    Shader(const std::string & vertex_path , const std::string & fragment_path);

private:
    void CreateProgram(unsigned int vertex, unsigned int fragment);
};

class ComputeShader : public ShaderBase
{
public:
    ComputeShader(const std::string & compute_path);

    void Compute(unsigned int groupnum_x = 1, unsigned int groupnum_y = 1, 
        unsigned int groupnum_z = 1, int barrier_type = 0x2000); //GL_BUFFER_UPDATE_BARRIER_BIT

private:
    void CreateProgram(unsigned int compute);
};