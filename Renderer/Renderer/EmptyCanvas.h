#pragma once
#include "Shader.h"

class EmptyCanvas
{
public:
    EmptyCanvas();
    ~EmptyCanvas();

    void Bind() const;
    void Unbind() const;
    void Draw(Shader& shader);

private:
    static float rect_vertices[];
    unsigned int VAO, VBO;
};