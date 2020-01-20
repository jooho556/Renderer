#pragma once

#include <vector>
#include "glm/glm.hpp"

class PerlinNoise
{
public:
    PerlinNoise();
    void GenerateNoiseTexture() const;

private:
    glm::vec2 GetPoint(float x, float y) const;
    unsigned int Hash(unsigned int x, unsigned int y) const;
    float GetPerlinNoise(const glm::vec2 & point) const;
    float FractalSum(glm::vec2 point, int layer_num, float & maximum_value) const;

    const unsigned int table_size;
    std::vector<glm::vec2> gradients;
    std::vector<unsigned int> permutation_table;
};

class PerlinNoise3D
{
public:
    PerlinNoise3D();
    std::vector<float> GetNoiseData() const;
    void GenerateImages(unsigned int layer_num) const;

private:
    glm::vec3 GetPoint(float x, float y, float z) const;
    unsigned int Hash(unsigned int x, unsigned int y, unsigned int z) const;
    float GetPerlinNoise(const glm::vec3& point) const;
    float FractalSum(glm::vec3 point, int layer_num, float& maximum_value) const;

    const unsigned int table_size;
    const unsigned int noise_side_length;
    std::vector<glm::vec3> gradients;
    std::vector<unsigned int> permutation_table;
};