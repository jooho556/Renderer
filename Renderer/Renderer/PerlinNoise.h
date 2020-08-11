#pragma once
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

class PerlinNoise // Perlin noise
{
public:
    PerlinNoise(ComputeShader& compute, int table_size = 128);
    ~PerlinNoise();
    void Draw(const Camera & cam, Shader & volume_shdr);

private:
    void BuildCube();
    void GenerateNoise(ComputeShader& compute);
    void Bind();
    void Unbind();

    Texture3D noise;
    unsigned int table_size;
    unsigned int noise_length;
    unsigned int vao, vbo, ebo;
    unsigned int pbo;    
};