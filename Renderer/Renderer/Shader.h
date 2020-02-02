#pragma once
#include <string>
#include <glm/glm.hpp>

enum class ShaderType
{
    Vertex,
    Fragment,
    Compute
};

class Shader
{
public:
    Shader(const std::string & vertex , const std::string & fragment);
    Shader(const std::string & compute);
    ~Shader();
    void Use() const;

    void SetFloat(const std::string & name, float value) const;
    void SetInt(const std::string & name, int value) const;
    void SetBoll(const std::string & name, bool value) const;
    void SetMat4(const std::string & name, const glm::mat4 & mat) const;
    void SetVec3(const std::string & name, const glm::vec3 & vec) const;
    void SetVec2(const std::string& name, const glm::vec2& vec) const;

    unsigned int m_programID;

private:
    unsigned int CreateShader(const std::string & source, ShaderType type);
    void CompileShader(unsigned int shader);
    void CreateProgram(unsigned int vertex, unsigned int fragment);
    void CreateProgram(unsigned int compute);
};