#pragma once
#include <string>
#include <glm/glm.hpp>

enum class ShaderType
{
    Vertex,
    Fragment
};

class Shader
{
public:
    Shader(const std::string & vertex = "Shaders/BasicVertex.glsl", 
        const std::string & fragment = "Shaders/BasicFragment.glsl");
    ~Shader();
    void Use();

    void SetFloat(const std::string & name, float value);
    void SetInt(const std::string & name, int value);
    void SetBoll(const std::string & name, bool value);
    void SetMat4(const std::string & name, const glm::mat4 & mat);
    void SetVec3(const std::string & name, const glm::vec3 & vec);

    unsigned int m_programID;

private:
    unsigned int CreateShader(const std::string & source, ShaderType type);
    void CompileShader(unsigned int shader);
    void CreateProgram(unsigned int vertex, unsigned int fragment);
};