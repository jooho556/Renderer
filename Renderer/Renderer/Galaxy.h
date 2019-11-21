#pragma once
#include "Texture.h"

class Shader;
class Camera;
class Old::Texture;
class Galaxy
{
public:
    Galaxy(const glm::vec3 & pos, const glm::vec3 & nebula_color, const glm::vec3 & star_color);
    ~Galaxy();

    void Draw(Shader * compute, Shader * nebula, Shader * star, const Camera * cam, const Old::Texture * tex);
    void SetPosition(const glm::vec3 & position) { pos = position; }
    void SetColor(const glm::vec3 & n_color) { nebula_color = n_color; }

private:
    void CreateGalaxy();
    float deVaucouleurs(float radius, float scale_length = 20.f);

    glm::vec3 pos;
    glm::vec3 nebula_color, star_color;

    unsigned int vao, pos_buf, mass_buf, e_info_buf, e_rinfo_buf;
    static int galaxy_num;
    static int particle_num;
};
