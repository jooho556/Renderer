#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Texture.h"
#include "Galaxy.h"
#include "Ellipse.h"
#include "stb_image_write.h"

namespace
{
    glm::vec3 RED = glm::vec3(1.f, 0.2f, 0.2f);
    glm::vec3 PURPLE = glm::vec3(0.8f, 0.2f, 0.8f);
    glm::vec3 SKY = glm::vec3(0.4f, 0.4f, 0.9f);
    glm::vec3 YELLOW = glm::vec3(0.8f, 0.8f, 0.2f);
    glm::vec3 Origin;
    constexpr int WINDOW_WIDTH = 1280;
    constexpr int WINDOW_HEIGHT = 900;
}

const glm::vec3 & GetColor()
{
    int remainder = rand() % 4;
    switch (remainder)
    {
    case 0: return RED;
    case 1: return PURPLE;
    case 2: return SKY;
    case 3: return YELLOW;
    default: return RED;
    }
}

void GenerateImage(const std::string & file_name, 
    Shader * compute, 
    Shader * star,
    const Camera * cam, 
    const Old::Texture * tex,
    const std::vector<Galaxy*> & universe, Window & window)
{
    window.StartDraw();
    for (auto & galaxy : universe)
        galaxy->Draw(compute, star, cam, tex);

    unsigned char * pixels = new unsigned char[3 * WINDOW_WIDTH * WINDOW_HEIGHT];
    glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    stbi_write_png(file_name.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 3, &pixels[0], 0);
    delete[] pixels;

    window.EndDraw();
}

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    Shader lighting_shader("Shaders/PongShading.vs", "Shaders/PongShading.fs");
    Shader basic_shader("Shaders/Basic.vs", "Shaders/Basic.fs");
    Shader depth_shader("Shaders/Basic.vs", "Shaders/DepthTest.fs");
    Shader cubemap_shader("Shaders/Cubemap.vs", "Shaders/Cubemap.fs");

    std::vector<std::string> space_cube = {
        "right.png",
        "left.png",
        "top.png",
        "bottom.png",
        "front.png",
        "back.png"
    };

    Shader particle_blackhole_shader("Shaders/ParticleBlackHole.cs");
    Shader particle_compute_shader("Shaders/Particle.cs");
    Shader particle_shader("Shaders/Particle.vs", "Shaders/Particle.fs");
    Skybox space(space_cube);

    const Camera & cam = window.GetCamera();
    Old::Texture particle_tex("Textures/RadialGradient.png");

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    std::vector<Galaxy*> universe;
    //Spherical coordinate
    for (int i = 0; i < 200; ++i)
    {
        float  theta = Angle::DegToRad(static_cast<float>(rand() % 180));
        float phi = Angle::DegToRad(static_cast<float>(rand() % 360));

        float z = 500 * cos(theta);
        float sin_theta = sin(theta);
        float x = 500 * sin_theta * cos(phi);
        float y = 500 * sin_theta * sin(phi);

        universe.push_back(new Galaxy(glm::vec3(x, y, z), GetColor(), 0.2f, 1.25f));
    }

    bool first_iteration = true;

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;

        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        


        space.Draw(cubemap_shader, cam);
        //for (auto & galaxy : universe)
        //    galaxy->Draw(&particle_compute_shader, &particle_shader, &cam, &particle_tex);
        //model.Draw(lighting_shader, cam);

        //if (first_iteration)
        //{
        //    Camera cam_front(WINDOW_WIDTH, WINDOW_HEIGHT, Origin, glm::vec3(0, 0, -1.f));
        //    GenerateImage("front.png", &particle_compute_shader, &particle_shader, &cam_front, &particle_tex, universe, window);

        //    Camera cam_back(WINDOW_WIDTH, WINDOW_HEIGHT, Origin, glm::vec3(0, 0, 1.f));
        //    GenerateImage("back.png", &particle_compute_shader, &particle_shader, &cam_back, &particle_tex, universe, window);

        //    Camera cam_right(WINDOW_WIDTH, WINDOW_HEIGHT, Origin, glm::vec3(1.f, 0, 0));
        //    GenerateImage("right.png", &particle_compute_shader, &particle_shader, &cam_right, &particle_tex, universe, window);

        //    Camera cam_left(WINDOW_WIDTH, WINDOW_HEIGHT, Origin, glm::vec3(-1.f, 0, 0));
        //    GenerateImage("left.png", &particle_compute_shader, &particle_shader, &cam_left, &particle_tex, universe, window);

        //    Camera cam_top(WINDOW_WIDTH, WINDOW_HEIGHT, Origin, glm::vec3(0, 1.f, 0));
        //    GenerateImage("top.png", &particle_compute_shader, &particle_shader, &cam_top, &particle_tex, universe, window);

        //    Camera cam_bottom(WINDOW_WIDTH, WINDOW_HEIGHT, Origin, glm::vec3(0, -1.f, 0));
        //    GenerateImage("bottom.png", &particle_compute_shader, &particle_shader, &cam_bottom, &particle_tex, universe, window);


        //    first_iteration = false;
        //}

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    for (auto & galaxy : universe)
        delete galaxy;

    return 0;
}