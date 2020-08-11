#include <vector>
#include "Texture.h"
#include "Shader.h"
#include "Ellipse.h"
#include "GL/glew.h"
#include "Galaxy.h"
#include "Camera.h"
#include "IMGUI/imgui.h"
#include "Window.h"

Galaxy::Galaxy(const glm::vec3 & pos, const glm::vec3 & color, 
    float ellipse_rad_a, float ellipse_eccentricity, const glm::vec3 & core_color)
    : pos(pos), color(color), core_color(core_color)
{
    CreateGalaxy(ellipse_rad_a, ellipse_eccentricity);
}

Galaxy::~Galaxy()
{
    Destroy();
}

void Galaxy::Rebuild(const GalaxyInfo& info)
{
    Destroy();
    surface_brightness = info.surface_brightness;
    particle_number_scale = info.particle_number_scale;
    CreateGalaxy(info.radius_a, info.eccentricity);
}

void Galaxy::Draw(ComputeShader * compute, Shader * star, 
    const Camera & cam, const Texture2D * tex)
{
    glBindVertexArray(vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particle_buf);
    tex->BindTexture();

    //Update position
    compute->Use();
    compute->SetFloat("PI", Angle::PI);
    compute->Compute(particle_num / 1000 + 1);

    //Draw nebula
    star->Use();
    star->SetMat4("model", glm::translate(pos) * rotation * scale);
    star->SetMat4("view", cam.GetViewMatrix());
    star->SetMat4("projection", cam.GetProjectionMatrix());
    star->SetVec3("color", color);
    star->SetFloat("alpha", 10.f);
    star->SetFloat("point_size_scale", 0.8f);
    glDrawArrays(GL_POINTS, 0, particle_num);

    //Draw star
    star->Use();
    star->SetVec3("color", core_color);
    star->SetFloat("alpha", 1.f);
    star->SetFloat("point_size_scale", 0.2f);
    glDrawArrays(GL_POINTS, 0, particle_num);

    glBindVertexArray(0);
}

struct ParticleInfo
{
    glm::vec3 position;
    float mass;
    float t;
    float ellise_rad_a;
    float ellise_rad_b;
    float rotate_angle;
};

void Galaxy::CreateGalaxy(float ellipse_rad_a, float ellipse_eccentricity)
{
    std::vector<ParticleInfo> particles;

    Ellipse ellipse(ellipse_rad_a, ellipse_eccentricity);
    for (float rotate_angle = 0.f; rotate_angle < Angle::PI * 1.5f; rotate_angle += 0.016f)
    {
        int point_num = static_cast<int>(deVaucouleurs(ellipse.GetRadA()) * particle_number_scale);
        particle_num += point_num;
        for (int i = 0; i < point_num; ++i)
        {           
            float t = rand() % 1000 / 1000.f;
            glm::vec3 point = ellipse.GetPoint(t);

            ParticleInfo info;
            info.position.x = point.x;
            info.position.y = point.y;
            info.position.z = 1; // w-component
            info.t = t;
            info.ellise_rad_a = ellipse.GetRadA();
            info.ellise_rad_b = ellipse.GetRadB();
            info.rotate_angle = rotate_angle;
            info.mass = static_cast<float>(rand() % 30 + 1);

            particles.push_back(info);
        }
        ellipse.AddRadius(0.1f);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &particle_buf);
    glBindBuffer(GL_ARRAY_BUFFER, particle_buf);
    glBufferData(GL_ARRAY_BUFFER, particle_num * sizeof(ParticleInfo), &particles[0], GL_DYNAMIC_DRAW);

    unsigned int stride = sizeof(ParticleInfo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 7));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Galaxy::Destroy()
{
    glDeleteBuffers(1, &particle_buf);
    particle_buf = 0;
    glDeleteVertexArrays(1, &vao);
    vao = 0;
    particle_num = 0;
}

float Galaxy::deVaucouleurs(float radius, float scale_length)
{
    return static_cast<float>(surface_brightness * std::exp(-7.669 * (powf(radius / scale_length, 0.25f) - 1)));
}