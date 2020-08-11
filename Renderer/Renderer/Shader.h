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
    void SetFloat(const std::string & name, float value);
    void SetInt(const std::string & name, int value);
    void SetBool(const std::string & name, bool value);
    void SetMat4(const std::string & name, const glm::mat4 & mat);
    void SetVec3(const std::string & name, const glm::vec3 & vec);
    void SetVec2(const std::string& name, const glm::vec2& vec);    
    
    const std::string& GetErrorLog() const { return error_log; }

    virtual ~ShaderBase() override;   
    virtual void Compile() = 0;

protected:
    ShaderBase() {}
    void Destroy();
    unsigned int CheckUniformLocation(const std::string & name);
    void CheckProgramLink(unsigned int program_id);
    unsigned int CreateShader(const std::string & source_path, unsigned int shader_type);
    void CompileShader(unsigned int shader);

    std::string error_log;
};

class Shader : public ShaderBase //Pipeline shader
{
public:
    Shader(const std::string & vertex_path , const std::string & fragment_path);
    virtual void Compile() override;
private:
    void Construct(const std::string& vertex_path, const std::string& fragment_path);
    void CreateProgram(unsigned int vertex, unsigned int fragment);
    std::string vert_path;
    std::string frag_path;
};

class ComputeShader : public ShaderBase
{
public:
    ComputeShader(const std::string & compute_path);

    virtual void Compile() override;
    void Compute(unsigned int groupnum_x = 1, unsigned int groupnum_y = 1, 
        unsigned int groupnum_z = 1, int barrier_type = 0x2000); //GL_BUFFER_UPDATE_BARRIER_BIT

private:
    void Construct(const std::string& compute_path);
    void CreateProgram(unsigned int compute);
    std::string comp_path;
};