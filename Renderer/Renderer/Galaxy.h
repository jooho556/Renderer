#pragma once
#include "Texture.h"

class ComputeShader;
class Shader;
class Camera;
class Texture;
class Galaxy
{
public:
    Galaxy(const glm::vec3 & pos, const glm::vec3 & nebula_color, float ellipse_rad_a, float ellipse_eccentricity, 
        const glm::vec3 & star_color = glm::vec3(1.f, 1.f, 1.f));
    ~Galaxy();

    void Draw(ComputeShader * compute, Shader * star, const Camera * cam, const Texture * tex);
    void SetPosition(const glm::vec3 & position) { pos = position; }
    void SetColor(const glm::vec3 & n_color) { nebula_color = n_color; }

private:
    void CreateGalaxy(float ellipse_rad_a, float ellipse_eccentricity);
    float deVaucouleurs(float radius, float scale_length = 20.f);

    glm::vec3 pos;
    glm::mat4 rotation = glm::mat4(1.f);
    glm::mat4 scale = glm::mat4(1.f);
    glm::vec3 nebula_color, star_color;

    unsigned int vao, particle_buf;
    int particle_num = 0;
};
