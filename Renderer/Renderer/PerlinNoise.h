#pragma once
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

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

    ComputeShader noise_compute;
    Shader volume_shdr;
    Texture3D noise;
    unsigned int table_size;
    unsigned int noise_length;
    unsigned int vao, vbo, ebo;
    unsigned int pbo;    
};