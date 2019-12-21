#include <algorithm>
#include <iostream>
#include "PerlinNoise.h"
#include "GL/glew.h"

PerlinNoise::PerlinNoise() : table_size(256)
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
