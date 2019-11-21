#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Texture.h"
#include "Galaxy.h"

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

    Shader particle_blackhole_shader("Shaders/ParticleBlackHole.cs");
    Shader particle_compute_shader("Shaders/Particle.cs");
    Shader particle_shader("Shaders/Particle.vs", "Shaders/Particle.fs");
    Shader nebula("Shaders/Nebula.vs", "Shaders/Nebula.fs");

    Skybox space(space_cube);

    const Camera & cam = window.GetCamera();
    Old::Texture particle_tex("Textures/RadialGradient.png");

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Galaxy galaxy1(glm::vec3(5, 0, 0), glm::vec3(0.5f, 0.5f, 0.9f), glm::vec3(1.f, 1.f, 1.f));

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;

        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        
        //space.Draw(cubemap_shader, cam);
        galaxy1.Draw(&particle_compute_shader, &nebula, &particle_shader, &cam, &particle_tex);
        galaxy1.SetPosition(glm::vec3(-100, 50, -200));
        galaxy1.SetColor(glm::vec3(0.6f, 0.2f, 0.6f));
        galaxy1.Draw(&particle_compute_shader, &nebula, &particle_shader, &cam, &particle_tex);
        galaxy1.SetPosition(glm::vec3(5, 0, 0));
        galaxy1.SetColor(glm::vec3(0.5f, 0.5f, 0.9f));
        //model.Draw(lighting_shader, cam);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }
    return 0;
}