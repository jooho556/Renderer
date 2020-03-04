#include "Window.h"
#include "Shader.h"
#include "Skybox.h"
#include "Texture.h"
#include "Galaxy.h"
#include "Ellipse.h"
#include "stb_image_write.h"
#include "PerlinNoise.h"

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    const Camera & cam = window.GetCamera();

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    int table_size = 128;
    PerlinNoise noise(table_size);

    Galaxy galaxy(glm::vec3(0, 0, -100), glm::vec3(0.4f, 0.4, 0.8f), 1., 1.2);

    Shader star_shdr("Shaders/Stars.vert", "Shaders/Stars.frag");


    std::vector<ComputeShader> s;
    std::vector<Texture2D> t;
    {
        ComputeShader galaxy_compute("Shaders/Particle.comp");
        s.push_back(galaxy_compute);
        Texture2D particle_tex("Textures/star.png");
        t.push_back(particle_tex);
    }
    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;
        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////

        noise.Draw(cam);
        galaxy.Draw(&s[0], &star_shdr, &cam, &t[0]);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}