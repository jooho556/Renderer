#include <ctime>
#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Texture.h"

namespace
{
    const float PI = 4.0f*atan(1.0f);
    float DegToRad(float degree) { return 2 * PI * degree / 360; }
}

float deVaucouleurs(float radius)
{
    float i0 = 1.f;
    float re = 10.f;
    return static_cast<float>(i0 * std::exp(-7.669 * (powf(radius / re, 0.25f) - 1)));
}

float RadToDeg(float rad) { return rad * 180 / PI; }

class Ellipse
{
public:
    Ellipse(float a, float eccen)
        : radius_a(a), eccentricity(eccen) {}

    glm::vec3 GetPoint(float t) const
    {
        float x = radius_a * std::cos(t * 2 * PI);
        //float x_vel = -2 * PI * radius_a * std::sin(t * 2 * PI);
        float y = radius_a * eccentricity * std::sin(t * 2 * PI);
        //float y_vel = 2 * PI * radius_a * eccentricity * std::cos(t * 2 * PI);
        return rotation * glm::vec4(x, y, 0, 1);
    }

    float GetRadA() const { return radius_a; }
    float GetRadB() const { return radius_a * eccentricity; }
    const glm::mat4 GetRotationMat() const { return rotation; }

    void Rotate(float rad)
    {
        rotation = glm::rotate(glm::mat4(1.f), rad, glm::vec3(0, 0, 1));
    }

    void AddRadius(float a)
    {
        radius_a += a;
    }

private:
    float radius_a = 1;
    float eccentricity = 1;
    glm::mat4 rotation = glm::mat4(1.f);
};

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    Shader lighting_shader("Shaders/PongShading.vs", "Shaders/PongShading.fs");
    Shader basic_shader("Shaders/Basic.vs", "Shaders/Basic.fs");
    Shader depth_shader("Shaders/Basic.vs", "Shaders/DepthTest.fs");
    Shader cubemap_shader("Shaders/Cubemap.vs", "Shaders/Cubemap.fs");

    std::vector<std::string> space_cube = {
        "Textures/Skybox/right.png",
        "Textures/Skybox/left.png",
        "Textures/Skybox/top.png",
        "Textures/Skybox/bottom.png",
        "Textures/Skybox/front.png",
        "Textures/Skybox/back.png"
    };

    /*****************************************************/
    /*****************************************************/
    srand(unsigned(time(0)));

    std::vector<float> ellipse_info;
    std::vector<float> particle_init_pos;
    std::vector<glm::mat4> rot_info;

    Ellipse ellipse(0.2f, 1.3f);
    for (float rotate_angle = 0.f; rotate_angle < PI * 1.5f; rotate_angle += 0.016f)
    {
        ellipse.Rotate(rotate_angle);
        int point_num = static_cast<int>(deVaucouleurs(ellipse.GetRadA()) * 100);
        for (int i = 0; i < point_num; ++i)
        {
            float t = rand() % 1000 / 1000.f;
            glm::vec3 pos = ellipse.GetPoint(t);
            particle_init_pos.push_back(pos.x);
            particle_init_pos.push_back(pos.y);
            particle_init_pos.push_back(0);
            particle_init_pos.push_back(1.f);

            ellipse_info.push_back(t);
            ellipse_info.push_back(ellipse.GetRadA());
            ellipse_info.push_back(ellipse.GetRadB());
            rot_info.push_back(ellipse.GetRotationMat());
        }
        ellipse.AddRadius(0.1f);
    }

    unsigned int particle_vao;
    glGenVertexArrays(1, &particle_vao);
    glBindVertexArray(particle_vao);

    //Position
    int particle_num = static_cast<int>(particle_init_pos.size()) / 4;
    int particle_buf_size = particle_num * sizeof(float) * 4;
    unsigned int particle_pos_buf;
    glGenBuffers(1, &particle_pos_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particle_pos_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_buf_size, &particle_init_pos[0], GL_DYNAMIC_DRAW);

    //Velocity
    std::vector<float> particle_init_vel(particle_buf_size, 0.f);
    unsigned int particle_vel_buf;
    glGenBuffers(1, &particle_vel_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particle_vel_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_buf_size, &particle_init_vel[0], GL_DYNAMIC_DRAW);

    //Mass
    std::vector<float> particle_mass_vec;
    for (int i = 0; i < particle_num; ++i)
        particle_mass_vec.push_back(static_cast<float>(rand() % 30 + 1));

    unsigned int particle_mass;
    glGenBuffers(1, &particle_mass);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particle_mass);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_num * sizeof(float), &particle_mass_vec[0], GL_DYNAMIC_DRAW);

    //ellipse info for calculating velocity
    unsigned int e_info;
    glGenBuffers(1, &e_info);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, e_info);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ellipse_info.size() * sizeof(float), &ellipse_info[0], GL_DYNAMIC_DRAW);

    //ellipse rotation info
    unsigned int e_rinfo;
    glGenBuffers(1, &e_rinfo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, e_rinfo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, rot_info.size() * sizeof(glm::mat4), &rot_info[0], GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, particle_pos_buf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    Shader particle_blackhole_shader("Shaders/ParticleBlackHole.cs");
    Shader particle_compute_shader("Shaders/Particle.cs");
    Shader particle_shader("Shaders/Particle.vs", "Shaders/Particle.fs");
    Shader nebula("Shaders/Nebula.vs", "Shaders/Nebula.fs");

    Skybox space(space_cube);
    Old::Texture particle("Textures/RadialGradient.png");

    const Camera & cam = window.GetCamera();

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;

        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        particle.BindTexture();

        //Compute shader for calculating velocity
        particle_compute_shader.Use();
        particle_compute_shader.SetFloat("PI", PI);
        glDispatchCompute(particle_num / 1000 + 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        //Nebula
        nebula.Use();
        nebula.SetFloat("tick", current_tick);
        nebula.SetMat4("model_to_view", cam.GetViewMatrix());
        nebula.SetMat4("projection", cam.GetProjectionMatrix());
        nebula.SetVec3("color", glm::vec3(0.6f, 0.2f, 0.6f));

        glBindVertexArray(particle_vao);
        glDrawArrays(GL_POINTS, 0, particle_num);
        glBindVertexArray(0);

        //Star
        particle_shader.Use();
        particle_shader.SetFloat("tick", current_tick);
        particle_shader.SetMat4("model_to_view", cam.GetViewMatrix());
        particle_shader.SetMat4("projection", cam.GetProjectionMatrix());
        particle_shader.SetVec3("color", glm::vec3(1.f, 1.f, 1.0f));

        glBindVertexArray(particle_vao);
        glDrawArrays(GL_POINTS, 0, particle_num);
        glBindVertexArray(0);

        //space.Draw(cubemap_shader, cam);
        //model.Draw(lighting_shader, cam);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    glDeleteVertexArrays(1, &particle_vao);
    glDeleteBuffers(1, &particle_pos_buf);
    glDeleteBuffers(1, &particle_vel_buf);
    glDeleteBuffers(1, &particle_mass);
    glDeleteBuffers(1, &e_info);
    glDeleteBuffers(1, &e_rinfo);
    return 0;
}