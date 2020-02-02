#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"
#include "Texture.h"
#include "Galaxy.h"
#include "Ellipse.h"
#include "stb_image_write.h"
#include "PerlinNoise.h"
#include "Constants.h"

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
        -1.f, 1.f, 0.f,
        1.f, 1.f, 0.f, 
        1.f, -1.f, 0.f,
        -1.f, 1.f, 0.f,
        1.f, -1.f, 0.f,
        -1.f, -1.f, 0.f
    };

    unsigned int rect_vao;
    glGenVertexArrays(1, &rect_vao);
    glBindVertexArray(rect_vao);

    unsigned int rect_vbo;
    glGenBuffers(1, &rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), &rect[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    Shader nebula_shader("Shaders/Nebula.vs", "Shaders/Nebula.fs");
    float seed = rand() * 1000 / 1000.f;

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;
        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        nebula_shader.Use();
        nebula_shader.SetVec2("iResolution", glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
        //nebula_shader.SetFloat("seed", seed);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}