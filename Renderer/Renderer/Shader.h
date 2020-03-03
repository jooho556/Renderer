#pragma once
#include <string>
#include <glm/glm.hpp>

class ShaderBase
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
    ShaderBase(const ShaderBase & sb) { Copy(sb); }
    ShaderBase(ShaderBase && sb) { Copy(sb); }
    virtual ~ShaderBase();
    ShaderBase& operator=(const ShaderBase & sb) = delete;

protected:
    void Copy(const ShaderBase & sb) {
        m_program_id = sb.m_program_id;
        sb.is_copied = true;
    }
    unsigned int CheckUniformLocation(const std::string & name) const;
    void CheckProgramLink(unsigned int program_id) const;
    unsigned int CreateShader(const std::string & source_path, unsigned int shader_type);
    void CompileShader(unsigned int shader);

    unsigned int m_program_id = -1;
    mutable bool is_copied = false;
};


class Shader : public ShaderBase
{
public:
    Shader(const std::string & vertex_path , const std::string & fragment_path);
    ~Shader() override {}

private:
    void CreateProgram(unsigned int vertex, unsigned int fragment);
};

class ComputeShader : public ShaderBase
{
public:
    ComputeShader(const std::string & compute_path);
    ~ComputeShader() override {}
    
private:
    void CreateProgram(unsigned int compute);
};