#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Texture;
class Shader;
class Drawable
{
public:
    Drawable(float vertices[], int num_v,
        unsigned int indices[], int num_i, Shader * shader = nullptr);
    ~Drawable();

    void Bind() const;
    void Unbind() const;

    unsigned int GetElementNum() const { return element_num; }
    unsigned int GetVertexNum() const { return vertex_num; }

    void SetShader(Shader * shader) { m_shader = shader; }
    Shader * GetShader() const { return m_shader; }

    void SetTexture(Texture * tex) { texture = tex; }
    Texture * GetTexture() const { return texture; }

    void SetPosition(const glm::vec3 & pos) { position = pos; }
    const glm::vec3 & GetPosition() const { return position; }
    glm::mat4 GetModel() const { return glm::translate(glm::mat4(1.f), position); }

    void SetColor(const glm::vec3 & _color) { color = _color; }
    const glm::vec3 & GetColor() const { return color; }

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int vertex_num = 0;
    unsigned int element_num = 0;

    glm::vec3 position;
    glm::vec3 color;

    Shader * m_shader = nullptr;
    Texture * texture = nullptr;
};