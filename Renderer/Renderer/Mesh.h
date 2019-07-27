#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Shader;
class Mesh
{
public:
    Mesh(const std::vector<Vertex> & vertices, const std::vector<unsigned int> & indices,
        const std::vector<Texture> & textures);
    void Draw(const Shader & shader) const;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

private:
    unsigned int VAO, VBO, EBO;
    void SetupMesh();
};