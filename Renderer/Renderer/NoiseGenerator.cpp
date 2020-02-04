#include "GL/glew.h"
#include "NoiseGenerator.h"

void NoiseGenerator::Compute()
{
    glDispatchCompute(table_size, table_size, table_size);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
