#include <algorithm>
#include <iostream>
#include "PerlinNoise.h"
#include "GL/glew.h"
#include "Constants.h"
#include "stb_image_write.h"

namespace
{
    unsigned int size = 256;
}

PerlinNoise::PerlinNoise() : table_size(size)
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
    //unsigned int p_initial_size = static_cast<unsigned int>(permutation_table.size());
    //for (unsigned int i = 0; i < p_initial_size; ++i)
    //    permutation_table.push_back(permutation_table[i]);
}

unsigned int PerlinNoise::Hash(unsigned int x, unsigned int y) const
{
    return permutation_table[(permutation_table[x] + y) % table_size];
}

glm::vec2 PerlinNoise::GetPoint(float x, float y) const
{
    unsigned int unit_x_length = SCREEN_WIDTH / table_size;
    unsigned int unit_y_length = SCREEN_HEIGHT/ table_size;
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
    int center_x = SCREEN_WIDTH / 2, center_y = SCREEN_HEIGHT / 2;
    float radius_sqr = center_x * 1.5f;

    float distance = static_cast<float>(std::sqrt((center_x - x) * (center_x - x) + (center_y - y) * (center_y - y)));
    if (distance >= radius_sqr) distance = radius_sqr;

    return glm::smoothstep(1.f, 0.f, distance / radius_sqr);
}

void PerlinNoise::GenerateNoiseTexture() const
{
    //Noise calculation
    std::vector<float> noise;
    float maximum_value = 0;
    for (unsigned int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (unsigned int x = 0; x < SCREEN_WIDTH; ++x)
        {
            float n = FractalSum(GetPoint(static_cast<float>(x), static_cast<float>(y)), 8, maximum_value);
            noise.push_back(n);
        }
    }

    //Normalize noise values
    std::for_each(noise.begin(), noise.end(), [maximum_value](float & noise) {noise /= maximum_value; });

    //Convert to 8 bit
    std::vector<unsigned char> pixels{ 0 };
    for (unsigned int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (unsigned int x = 0; x < SCREEN_WIDTH; ++x)
        {
            int index = SCREEN_WIDTH * y + x;
            float n = FadeFromCenter(x, y) * std::pow(noise[index], 4);
            unsigned char n_char = static_cast<unsigned char>(n * 255);
            pixels.push_back(n_char);
            pixels.push_back(n_char);
            pixels.push_back(n_char);
        }
    }
    
    stbi_write_png("PerlinNoise.png", SCREEN_WIDTH, SCREEN_HEIGHT, 3, &pixels[0], 0);
}


///////////////////////////////////////////////////
//3D version

PerlinNoise3D::PerlinNoise3D() : table_size(size), noise_side_length(256)
{
    //Generate random gradients
    for (unsigned int i = 0; i < table_size; ++i)
    {
        float x = ((rand() % 1000) / 1000.f) * 2 - 1;
        float y = ((rand() % 1000) / 1000.f) * 2 - 1;
        float z = ((rand() % 1000) / 1000.f) * 2 - 1;
        gradients.push_back(glm::normalize(glm::vec3(x, y, z)));
        permutation_table.push_back(i);
    }

    //Shuffle p table for hashing
    std::random_shuffle(permutation_table.begin(), permutation_table.end());

    BuildTexture3D();
}

PerlinNoise3D::~PerlinNoise3D()
{
    if (texture_id != 0)
        glDeleteTextures(1, &texture_id);
}

void PerlinNoise3D::Bind() const
{
    glBindTexture(GL_TEXTURE_3D, texture_id);
}

glm::vec3 PerlinNoise3D::GetPoint(float x, float y, float z) const
{
    unsigned int grid_side_length = noise_side_length / table_size;
    return glm::vec3(x / grid_side_length, y / grid_side_length, z / grid_side_length);
}

unsigned int PerlinNoise3D::Hash(unsigned int x, unsigned int y, unsigned int z) const
{
    return permutation_table[(permutation_table[(permutation_table[x] + y) % table_size] + z) % table_size];
}

float PerlinNoise3D::GetPerlinNoise(const glm::vec3& p) const
{
    int table_size_mask = table_size - 1;

    //Eight corners
    int xi0 = static_cast<int>(floor(p.x)) & table_size_mask;
    int yi0 = static_cast<int>(floor(p.y)) & table_size_mask;
    int zi0 = static_cast<int>(floor(p.z)) & table_size_mask;
    int xi1 = (xi0 + 1) & table_size_mask;
    int yi1 = (yi0 + 1) & table_size_mask;
    int zi1 = (zi0 + 1) & table_size_mask;

    //Fractional part in local grid
    float tx = p.x - static_cast<int>(floor(p.x));
    float ty = p.y - static_cast<int>(floor(p.y));
    float tz = p.z - static_cast<int>(floor(p.z));

    float t1 = glm::smoothstep(0.f, 1.f, tx);
    float t2 = glm::smoothstep(0.f, 1.f, ty);
    float t3 = glm::smoothstep(0.f, 1.f, tz);

    //Getting gradient vectors from every corners
    glm::vec3 grad_bot_left_lower = gradients[Hash(xi0, yi0, zi0)];
    glm::vec3 grad_bot_right_lower = gradients[Hash(xi1, yi0, zi0)];
    glm::vec3 grad_top_right_lower = gradients[Hash(xi1, yi1, zi0)];
    glm::vec3 grad_top_left_lower = gradients[Hash(xi0, yi1, zi0)];
    glm::vec3 grad_bot_left_upper = gradients[Hash(xi0, yi0, zi1)];
    glm::vec3 grad_bot_right_upper = gradients[Hash(xi1, yi0, zi1)];
    glm::vec3 grad_top_right_upper = gradients[Hash(xi1, yi1, zi1)];
    glm::vec3 grad_top_left_upper = gradients[Hash(xi0, yi1, zi1)];

    //Generate vectors going from every corner to p
    float x0 = tx;
    float x1 = tx - 1;
    float y0 = ty;
    float y1 = ty - 1;
    float z0 = tz;
    float z1 = tz - 1;

    glm::vec3 bot_left_lower_to_p = glm::vec3(x0, y0, z0);
    glm::vec3 bot_right_lower_to_p = glm::vec3(x1, y0, z0);
    glm::vec3 top_right_lower_to_p = glm::vec3(x1, y1, z0);
    glm::vec3 top_left_lower_to_p = glm::vec3(x0, y1, z0);
    glm::vec3 bot_left_upper_to_p = glm::vec3(x0, y0, z1);
    glm::vec3 bot_right_upper_to_p = glm::vec3(x1, y0, z1);
    glm::vec3 top_right_upper_to_p = glm::vec3(x1, y1, z1);
    glm::vec3 top_left_upper_to_p = glm::vec3(x0, y1, z1);

    //Dot product
    float bot_left_lower_val = dot(grad_bot_left_lower, bot_left_lower_to_p);
    bot_left_lower_val = (bot_left_lower_val + 1) / 2.f;
    float bot_right_lower_val = dot(grad_bot_right_lower, bot_right_lower_to_p);
    bot_right_lower_val = (bot_right_lower_val + 1) / 2.f;
    float top_right_lower_val = dot(grad_top_right_lower, top_right_lower_to_p);
    top_right_lower_val = (top_right_lower_val + 1) / 2.f;
    float top_left_lower_val = dot(grad_top_left_lower, top_left_lower_to_p);
    top_left_lower_val = (top_left_lower_val + 1) / 2.f;
    float bot_left_upper_val = dot(grad_bot_left_upper, bot_left_upper_to_p);
    bot_left_upper_val = (bot_left_upper_val + 1) / 2.f;
    float bot_right_upper_val = dot(grad_bot_right_upper, bot_right_upper_to_p);
    bot_right_upper_val = (bot_right_upper_val + 1) / 2.f;
    float top_right_upper_val = dot(grad_top_right_upper, top_right_upper_to_p);
    top_right_upper_val = (top_right_upper_val + 1) / 2.f;
    float top_left_upper_val = dot(grad_top_left_upper, top_left_upper_to_p);
    top_left_upper_val = (top_left_upper_val + 1) / 2.f;

    //trilinear interpolation
    float bottom_edge_lower_x = lerp(bot_left_lower_val, bot_right_lower_val, t1);
    float top_edge_lower_x = lerp(top_left_lower_val, top_right_lower_val, t1);
    float lower_y = lerp(bottom_edge_lower_x, top_edge_lower_x, t2);

    float bottom_edge_upper_x = lerp(bot_left_upper_val, bot_right_upper_val, t1);
    float top_edge_upper_x = lerp(top_left_upper_val, top_right_upper_val, t1);
    float upper_y = lerp(bottom_edge_upper_x, top_edge_upper_x, t2);

    return lerp(lower_y, upper_y, t3);
}

float PerlinNoise3D::FractalSum(glm::vec3 point, int layer_num, float& maximum_value) const
{
    float n = 0.f;
    for(int i = 0; i < layer_num; ++i)
    {
        n += GetPerlinNoise(point);
        point /= 2.f;
    }
    if (maximum_value < n) maximum_value = n;
    return n;
}

void PerlinNoise3D::BuildTexture3D()
{
    std::vector<float> noise_data;
    float maximum_value = 0;
    for(float z = 0; z < noise_side_length; ++z)
    {
        for (float y = 0; y < noise_side_length; ++y)
        {
            for (float x = 0; x < noise_side_length; ++x)
            {
                float n = FractalSum(GetPoint(x, y, z), 8, maximum_value);
                noise_data.push_back(n);
                noise_data.push_back(n);
                noise_data.push_back(n);
            }
        }
    }

    std::for_each(noise_data.begin(), noise_data.end(),
        [maximum_value](float& noise)
        {
            noise /= maximum_value; 
            noise = std::powf(noise, 10);
        });

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_3D, texture_id);

    //Texture Wrapping (0 ~ 1)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    //Texture filtering - sampling
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, noise_side_length, noise_side_length, noise_side_length, 0, GL_RGB, GL_FLOAT, noise_data.data());
}
