#pragma once
 
class NoiseGenerator // Perlin noise
{
public:
    NoiseGenerator() {}

private:
    void Compute();
    unsigned int table_size;
};