#include <GL/glew.h>
#include "Mesh.h"
#include "Shader.h"

Mesh::Mesh(const std::vector<Vertex> & vertices, const std::vector<unsigned int> & indices,
    const std::vector<Texture> & textures)
    : vertices(vertices), indices(indices), textures(textures)
{
    SetupMesh();
}

void Mesh::Draw(const Shader & shader) const
{
    unsigned int diffuse_num = 1;
    unsigned int specular_num = 1;

    for (unsigned int i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        
        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse")
            number = std::to_string(diffuse_num++);
        else if (name == "texture_specular")
            number = std::to_string(specular_num++);

        //shader.SetFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    //Draw
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    //Positions
    int vertex_size = sizeof(Vertex);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);

    //Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)offsetof(Vertex, normal));

    //Texture Coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void*)offsetof(Vertex, texcoord));

    glBindVertexArray(0);
}
