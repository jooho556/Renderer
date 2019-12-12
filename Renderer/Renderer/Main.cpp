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
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 1024;
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
    Skybox space(space_cube);
    Model sphere("Models/sphere/sphere.obj");

    const Camera & cam = window.GetCamera();
    Old::Texture particle_tex("Textures/RadialGradient.png");

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    std::vector<float> stars;
    int stars_num = 10000;
    for (int i = 0; i < stars_num; ++i)
    {
        float  theta = Angle::DegToRad(static_cast<float>(rand() % 18000) / 100.f);
        float phi = Angle::DegToRad(static_cast<float>(rand() % 36000) / 100.f);

        float z = 50 * cos(theta);
        float sin_theta = sin(theta);
        float x = 50 * sin_theta * cos(phi);
        float y = 50 * sin_theta * sin(phi);

        stars.push_back(x);
        stars.push_back(y);
        stars.push_back(z);
    }

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, stars.size() * sizeof(float), stars.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);
    particle_tex.BindTexture();

    bool first_iteration = true;

    float rect[] = 
    {
        -1.f, 1.f, 0.f, 0.f, 1.f,
        1.f, 1.f, 0.f, 1.f, 1.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        -1.f, 1.f, 0.f, 0.f, 1.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        -1.f, -1.f, 0.f, 0.f, 0.f
    };

    unsigned int rect_vao;
    glGenVertexArrays(1, &rect_vao);
    glBindVertexArray(rect_vao);

    unsigned int rect_vbo;
    glGenBuffers(1, &rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), &rect[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    Old::Texture noise_src("Noise.png");
    Old::Texture star_src("Stars.png");
    Shader nebula_shader("Shaders/Nebula.vs", "Shaders/Nebula.fs");

    float permutation[] = { 151,160,137,91,90,15,					// Hash lookup table as defined by Ken Perlin.  This is a randomly
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,	// arranged array of all numbers from 0-255 inclusive.
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    std::vector<float> p;
    for (int i = 0; i < 512; ++i)
        p.push_back(permutation[i % 256]);

    unsigned int hash_buffer;
    glGenBuffers(1, &hash_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, hash_buffer, 0);
    glBufferData(GL_SHADER_STORAGE_BUFFER, p.size() * sizeof(float), p.data(), GL_STATIC_DRAW);

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;
        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        //glBindVertexArray(vao);
        //particle_tex.BindTexture();
        //star_shader.Use();
        //star_shader.SetMat4("view", cam.GetViewMatrix());
        //star_shader.SetMat4("projection", cam.GetProjectionMatrix());

        nebula_shader.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, star_src.GetTextureHandle());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, noise_src.GetTextureHandle());

        nebula_shader.SetInt("stars", 0);
        nebula_shader.SetInt("noise_source", 1);

        nebula_shader.SetFloat("screen_width", static_cast<float>(WINDOW_WIDTH));
        nebula_shader.SetFloat("screen_height", static_cast<float>(WINDOW_HEIGHT));

        //if (first_iteration)
        //{
        //    unsigned char * pixels = new unsigned char[3 * WINDOW_WIDTH * WINDOW_HEIGHT];
        //    glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        //    stbi_write_png("Noise_src.png", WINDOW_WIDTH, WINDOW_HEIGHT, 3, &pixels[0], 0);
        //    delete[] pixels;

        //    first_iteration = false;
        //}

        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glDrawArrays(GL_POINTS, 0, stars_num);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}