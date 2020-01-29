#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Texture.h"
#include "Galaxy.h"
#include "Ellipse.h"
#include "stb_image_write.h"
#include "PerlinNoise.h"

namespace
{
    glm::vec3 RED = glm::vec3(1.f, 0.2f, 0.2f);
    glm::vec3 PURPLE = glm::vec3(0.8f, 0.2f, 0.8f);
    glm::vec3 SKY = glm::vec3(0.4f, 0.4f, 0.9f);
    glm::vec3 YELLOW = glm::vec3(0.8f, 0.8f, 0.2f);
    glm::vec3 Origin;
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 1024;
}

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    Shader lighting_shader("Shaders/PongShading.vs", "Shaders/PongShading.fs");
    Shader basic_shader("Shaders/Basic.vs", "Shaders/Basic.fs");
    Shader depth_shader("Shaders/Basic.vs", "Shaders/DepthTest.fs");
    Shader cubemap_shader("Shaders/Cubemap.vs", "Shaders/Cubemap.fs");
    Shader environment_shader("Shaders/Environment.vs", "Shaders/Environment.fs");

    std::vector<std::string> space_cube = {
        "Textures/Skybox/right.png",
        "Textures/Skybox/left.png",
        "Textures/Skybox/top.png",
        "Textures/Skybox/bottom.png",
        "Textures/Skybox/front.png",
        "Textures/Skybox/back.png"
    };

    Shader particle_blackhole_shader("Shaders/ParticleBlackHole.cs");
    Shader particle_compute_shader("Shaders/Particle.cs");
    Shader particle_shader("Shaders/Particle.vs", "Shaders/Particle.fs");
    Shader star_shader("Shaders/Stars.vs", "Shaders/Stars.fs");
    Shader noise_shader("Shaders/Noise.vs", "Shaders/Noise.fs");
    Shader volume_shader("Shaders/Volume.vs", "Shaders/Volume.fs");
    Skybox space(space_cube);
    Model sphere("Models/sphere/sphere.obj");

    const Camera & cam = window.GetCamera();
    Old::Texture particle_tex("Textures/RadialGradient.png");

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    float rect[] =
    {
        0.f, 0.f, 0.f,  0.f, 0.f, 0.f,
        1.f, 0.f, 0.f,  1.f, 0.f, 0.f,
        1.f, 1.f, 0.f,  1.f, 1.f, 0.f,
        0.f, 1.f, 0.f,  0.f, 1.f, 0.f,
        0.f, 0.f, 1.f,  0.f, 0.f, 1.f,
        1.f, 0.f, 1.f,  1.f, 0.f, 1.f,
        1.f, 1.f, 1.f,  1.f, 1.f, 1.f,
        0.f, 1.f, 1.f,   0.f, 1.f, 1.f
    };

    unsigned int indices[] = {
        0, 1, 4,
        4, 1, 5,
        1, 2, 5,
        5, 2, 6,
        6, 2, 3,
        6, 3, 7,
        7, 3, 0,
        7, 0, 4,
        7, 4, 5,
        7, 5, 6,
        0, 2, 3,
        0, 1, 2
    };

    unsigned int cube_vao;
    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);

    unsigned int cube_vbo;
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), &rect[0], GL_STATIC_DRAW);

    unsigned int cube_ebo;
    glGenBuffers(1, &cube_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    Old::Texture transfer_func("Textures/cool-warm-paraview.png");
    transfer_func.BindTexture();

    PerlinNoise3D perlin3d;
    perlin3d.Bind();

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;
        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        volume_shader.Use();
        volume_shader.SetMat4("view", cam.GetViewMatrix());
        volume_shader.SetMat4("projection", cam.GetProjectionMatrix());
        volume_shader.SetVec3("eye_pos", cam.GetPosition());
        volume_shader.SetFloat("time", current_tick);
        volume_shader.SetVec3("u_color", glm::vec3(0.2f, 0.8f, 0.2f));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS, 0, stars_num);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}