#include <algorithm>
#include <iostream>
#include "PerlinNoise.h"
#include "GL/glew.h"
#include "Constants.h"
#include "stb_image_write.h"

PerlinNoise::PerlinNoise() : table_size(512)
{
    //Generate random gradients
    for (unsigned int i = 0; i < table_size; ++i)
    {
        float x = ((rand() % 1000) / 1000.f) * 2 - 1;
        float y = ((rand() % 1000) / 1000.f) * 2 - 1;
        gradients.push_back(glm::normalize(glm::vec2(x, y)));
        permutation_table.push_back(i);
    }

    //Shuffle p table for hashing
    std::random_shuffle(permutation_table.begin(), permutation_table.end());

    //Duplicate the elements to prevent array index overflow (256 -> 512) during hashing
    unsigned int p_initial_size = static_cast<unsigned int>(permutation_table.size());
    for (unsigned int i = 0; i < p_initial_size; ++i)
        permutation_table.push_back(permutation_table[i]);
}

unsigned int PerlinNoise::Hash(unsigned int x, unsigned int y) const
{
    return permutation_table[permutation_table[x] + y];
}

glm::vec2 PerlinNoise::GetPoint(float x, float y) const
{
    unsigned int unit_x_length = Constants::SCREEN_WIDTH / table_size;
    unsigned int unit_y_length = Constants::SCREEN_HEIGHT/ table_size;
    return glm::vec2(x / unit_x_length, y / unit_y_length);
}

float lerp(float a, float b, float t) { return a + t * (b - a); }

float PerlinNoise::GetPerlinNoise(const glm::vec2 & p) const
{
    int table_size_mask = table_size - 1;

    //Four corners
    int xi0 = (int(floor(p.x))) & table_size_mask;
    int yi0 = (int(floor(p.y))) & table_size_mask;
    int xi1 = (xi0 + 1) & table_size_mask;
    int yi1 = (yi0 + 1) & table_size_mask;

    //Fractional part in local grid
    float tx = p.x - (int(floor(p.x)));
    float ty = p.y - (int(floor(p.y)));

    float t1 = glm::smoothstep(0.f, 1.f, tx);
    float t2 = glm::smoothstep(0.f, 1.f, ty);

    //Getting gradient of each corner
    glm::vec2 g_bot_left = gradients[Hash(xi0, yi0)];
    glm::vec2 g_bot_right = gradients[Hash(xi1, yi0)];
    glm::vec2 g_top_right = gradients[Hash(xi1, yi1)];
    glm::vec2 g_top_left = gradients[Hash(xi0, yi1)];

    //Generate vectors going from each corner to p
    float x0 = tx;
    float x1 = tx - 1;
    float y0 = ty;
    float y1 = ty - 1;

    glm::vec2 bot_left_to_p = glm::vec2(x0, y0);
    glm::vec2 bot_right_to_p = glm::vec2(x1, y0);
    glm::vec2 top_right_to_p = glm::vec2(x1, y1);
    glm::vec2 top_left_to_p = glm::vec2(x0, y1);

    //Dot product
    float bot_left_val = dot(g_bot_left, bot_left_to_p);
    bot_left_val = (bot_left_val + 1) / 2.f;
    float bot_right_val = dot(g_bot_right, bot_right_to_p);
    bot_right_val = (bot_right_val + 1) / 2.f;
    float top_right_val = dot(g_top_right, top_right_to_p);
    top_right_val = (top_right_val + 1) / 2.f;
    float top_left_val = dot(g_top_left, top_left_to_p);
    top_left_val = (top_left_val + 1) / 2.f;

    //Bi-linear interpolation
    float bottom_edge_x = lerp(bot_left_val, bot_right_val, t1);
    float top_edge_x = lerp(top_left_val, top_right_val, t1);
    float final_result = lerp(bottom_edge_x, top_edge_x, t2);

    return final_result;
}

float PerlinNoise::FractalSum(glm::vec2 point, int layer_num, float & maximum_value) const
{
    float n = 0;
    for (int i = 0; i < layer_num; ++i)
    {
        n += GetPerlinNoise(point);
        point /= 2.f;
    }
    if (maximum_value < n) maximum_value = n;
    return n;
}

float FadeFromCenter(unsigned int x, unsigned int y)
{
    int center_x = Constants::SCREEN_WIDTH / 2, center_y = Constants::SCREEN_HEIGHT / 2;
    float radius_sqr = center_x * 1.5f;

    float distance = std::sqrt((center_x - x) * (center_x - x) + (center_y - y) * (center_y - y));
    if (distance >= radius_sqr) distance = radius_sqr;

    return glm::smoothstep(1.f, 0.f, distance / radius_sqr);
}

void PerlinNoise::GenerateNoiseTexture() const
{
    //Noise calculation
    std::vector<float> noise;
    float maximum_value = 0;
    for (unsigned int y = 0; y < Constants::SCREEN_HEIGHT; ++y)
    {
        for (unsigned int x = 0; x < Constants::SCREEN_WIDTH; ++x)
        {
            float n = FractalSum(GetPoint(x, y), 8, maximum_value);
            noise.push_back(n);
        }
    }

    //Normalize noise values
    std::for_each(noise.begin(), noise.end(), [maximum_value](float & noise) {noise /= maximum_value; });

    //Convert to 8 bit
    std::vector<unsigned char> pixels{ 0 };
    for (unsigned int y = 0; y < Constants::SCREEN_HEIGHT; ++y)
    {
        for (unsigned int x = 0; x < Constants::SCREEN_WIDTH; ++x)
        {
            int index = Constants::SCREEN_WIDTH * y + x;
            float n = FadeFromCenter(x, y) * std::pow(noise[index], 4);
            unsigned char n_char = static_cast<unsigned char>(n * 255);
            pixels.push_back(n_char);
            pixels.push_back(n_char);
            pixels.push_back(n_char);
        }
    }
    
    stbi_write_png("PerlinNoise.png", Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT, 3, &pixels[0], 0);
}