#include <vector>
#include <algorithm>
#include "GL/glew.h"
#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(int table_size)
    : table_size(table_size), noise_length(table_size * 2), 
    noise_compute("Shaders/NoiseGenerator.comp"),
    volume_shdr("Shaders/volume.vert", "Shaders/volume.frag")
{
    BuildCube();
    BuildTexture();
}

PerlinNoise::~PerlinNoise()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &pbo);
}

void PerlinNoise::Draw(const Camera& cam)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    Bind();

    volume_shdr.Use();
    volume_shdr.SetMat4("view", cam.GetViewMatrix());
    volume_shdr.SetMat4("projection", cam.GetProjectionMatrix());
    volume_shdr.SetVec3("eye_pos", cam.GetPosition());    
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    Unbind();
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);    
}

void PerlinNoise::Bind()
{
    glBindVertexArray(vao);
    noise.BindTexture();
}

void PerlinNoise::Unbind()
{
    glBindVertexArray(0);
    noise.UnbindTexture();
}

void PerlinNoise::BuildCube()
{
    float rect[] =
    {
        0.f, 0.f, 0.f,  0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,  1.f, 0.f, 0.f,
        1.f, 1.f, 0.f,  1.f, 1.f, 0.f,
        0.f, 1.f, 0.f,  0.f, 1.f, 0.f,
        0.f, 0.f, 1.f,  0.f, 0.f, 1.f,
        1.f, 0.f, 1.f,  1.f, 0.f, 1.f,
        1.f, 1.f, 1.f,  1.f, 1.f, 1.f,
        0.f, 1.f, 1.f,   0.f, 1.f, 1.f
    };

    unsigned int indices[] = {
        0, 1, 4,
        4, 1, 5,
        1, 2, 5,
        5, 2, 6,
        2, 3, 6,
        6, 3, 7,
        3, 0, 7,
        7, 0, 4,
        4, 5, 7,
        7, 5, 6,
        3, 2, 0,
        0, 2, 1
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), &rect[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
}

void PerlinNoise::BuildTexture()
{
    GenerateNoise();
  }

void PerlinNoise::GenerateNoise()
{
    std::vector<glm::vec4> gradients;
    std::vector<unsigned int> permutation_table;
    //Generate random gradients
    for (unsigned int i = 0; i < table_size; ++i)
    {
        float x = ((rand() % 1000) / 1000.f) * 2 - 1;
        float y = ((rand() % 1000) / 1000.f) * 2 - 1;
        float z = ((rand() % 1000) / 1000.f) * 2 - 1;
        glm::vec3 n = glm::normalize(glm::vec3(x, y, z));
        gradients.push_back(glm::vec4(n.x, n.y, n.z, 0));
        permutation_table.push_back(i);
    }

    //Shuffle p table for hashing
    std::random_shuffle(permutation_table.begin(), permutation_table.end());

    //PBO
    int texture_byte_size = static_cast<int>(std::pow(noise_length, 3));
    glGenBuffers(1, &pbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pbo);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(float) * texture_byte_size, 0, 0);

    unsigned int grad_buf, permutation_buf;
    glGenBuffers(1, &grad_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grad_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * gradients.size(),
        gradients.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &permutation_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, permutation_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * permutation_table.size(),
        permutation_table.data(), GL_STATIC_DRAW);

    //Use compute shader for noise generation
    noise_compute.Use();
    noise_compute.SetInt("table_size", table_size);
    noise_compute.SetInt("noise_side_length", noise_length);
    noise_compute.Compute(table_size, table_size, table_size);

    //Send the noise buffer to bound texture
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
    noise.LoadTexture(noise_length, noise_length, noise_length,GL_RED, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
