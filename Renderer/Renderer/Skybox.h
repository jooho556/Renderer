#pragma once
#include <vector>
#include <string>
#include "Shader.h"
#include "Camera.h"

class Skybox
{
public:
    Skybox(const std::vector<std::string> & faces_path);
    ~Skybox();

    void Draw(Shader & shader, const Camera & camera) const;

private:
    unsigned int LoadCubemap(const std::vector<std::string> & faces_path);

    unsigned int VAO = 0, VBO = 0;
    unsigned int texture_id = 0;
    static float skybox_vertices[];
};