#pragma once

#include <vector>
#include "glm/glm.hpp"

class PerlinNoise
{
public:
    PerlinNoise();

    unsigned int GetTableSize() const { return table_size; }
    const std::vector<glm::vec2> & GetGradients() const { return gradients; }
    const std::vector<unsigned int> & GetPermutationTable() const { return permutation_table; }

private:
    const unsigned int table_size = 256;
    std::vector<glm::vec2> gradients;
    std::vector<unsigned int> permutation_table;
};