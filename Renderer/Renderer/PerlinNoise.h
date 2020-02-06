#pragma once
#include "Shader.h"
#include "Camera.h"

class PerlinNoise // Perlin noise
{
public:
    PerlinNoise(int table_size);
    ~PerlinNoise();
    void Draw(const Camera & cam);

private:
    void BuildCube();
    void BuildTexture();
    void GenerateNoise();
    void Bind();
    void Unbind();

    Shader noise_compute;
    Shader volume_shdr;
    unsigned int table_size;
    unsigned int noise_length;
    unsigned int vao, vbo, ebo;
    unsigned int noise_texture;
    unsigned int pbo;    
};