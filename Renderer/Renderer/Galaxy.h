#pragma once

class ComputeShader;
class Shader;
class Camera;
class Texture2D;

struct GalaxyInfo
{
    glm::vec3 col = glm::vec3(1.);
    glm::vec3 core_col = glm::vec3(1.);
    float radius_a = .1f;
    float eccentricity = 1.2f;
    float surface_brightness = 1.f;
    int particle_number_scale = 1;    
};

class Galaxy
{
public:
    Galaxy(const glm::vec3 & pos, const glm::vec3 & color, float ellipse_rad_a, float ellipse_eccentricity, 
        const glm::vec3 & core_color = glm::vec3(1.f, 1.f, 1.f));
    ~Galaxy();
    void Rebuild(const GalaxyInfo& info);

    void Draw(ComputeShader * compute, Shader * star, const Camera & cam, const Texture2D * tex);
    void SetPosition(const glm::vec3 & position) { pos = position; }
    void SetCoreColor(const glm::vec3& new_color) { core_color = new_color; }
    void SetColor(const glm::vec3 & new_color) { color = new_color; }

private:
    void CreateGalaxy(float ellipse_rad_a, float ellipse_eccentricity);
    void Destroy();
    float deVaucouleurs(float radius, float scale_length = 20.f);

    glm::vec3 pos;
    glm::mat4 rotation = glm::mat4(1.f);
    glm::mat4 scale = glm::mat4(1.f);
    glm::vec3 color, core_color;

    int particle_number_scale = 50;
    float surface_brightness = 1.;
    unsigned int vao, particle_buf;
    int particle_num = 0;
};
