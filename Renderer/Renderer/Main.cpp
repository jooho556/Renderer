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
    const Camera & cam = window.GetCamera();

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

    Shader volume_shdr("Shaders/volume.vs", "Shaders/volume.fs");
    //PerlinNoise3D perlin3d;
    //perlin3d.Bind();

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;
        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        volume_shdr.Use();
        volume_shdr.SetMat4("view", cam.GetViewMatrix());
        volume_shdr.SetMat4("projection", cam.GetProjectionMatrix());
        volume_shdr.SetVec3("eye_pos", cam.GetPosition());
        volume_shdr.SetFloat("time", current_tick);
        volume_shdr.SetVec3("u_color", glm::vec3(0.2f, 0.8f, 0.2f));

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS, 0, stars_num);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}