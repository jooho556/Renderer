#include "Window.h"
#include "Shader.h"
#include "Model.h"
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

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
        2, 3, 6,
        6, 3, 7,
        3, 0, 7,
        7, 0, 4,
        4, 5, 7,
        7, 5, 6,
        3, 2, 0,
        0, 2, 1
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
    Old::Texture transfer_func("Textures/cool-warm-paraview.png");
    transfer_func.BindTexture();

    //Shader nebula_shdr("Shaders/Nebula.vs", "Shaders/Nebula.fs");

    //////////////////////////////
    //3D Texture
     int table_size = 2;
     int noise_side_length = 4;
     int invocation_num = noise_side_length / table_size;
     int texture_byte_size = static_cast<unsigned int>(std::pow(table_size, 3) * std::pow(invocation_num, 3));

     Shader noise_compute_shdr("Shaders/NoiseGenerator.cshdr");

    unsigned int noise3d;
    glGenTextures(1, &noise3d);
    glBindTexture(GL_TEXTURE_3D, noise3d);

    //Texture Wrapping (0 ~ 1)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    //Texture filtering - sampling
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //PBO
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, texture_byte_size, 0, 0);

    std::vector<glm::vec4> gradients;
    std::vector<unsigned int> permutation_table;
    //Generate random gradients
    for (int i = 0; i < table_size; ++i)
    {
        float x = ((rand() % 1000) / 1000.f) * 2 - 1;
        float y = ((rand() % 1000) / 1000.f) * 2 - 1;
        float z = ((rand() % 1000) / 1000.f) * 2 - 1;
        glm::vec3 n = glm::normalize(glm::vec3(x, y, z));
        gradients.push_back(glm::vec4(n.x, n.y, n.z, 0));
        permutation_table.push_back(i);
    }

    //Shuffle p table for hashing
    std::random_shuffle(permutation_table.begin(), permutation_table.end());

    unsigned int grad_buf, permutation_buf;
    glGenBuffers(1, &grad_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, grad_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4)* gradients.size(),
        gradients.data(), GL_STATIC_DRAW);
    
    glGenBuffers(1, &permutation_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, permutation_buf);    
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int)* permutation_table.size(),
        permutation_table.data(), GL_STATIC_DRAW);
    
    noise_compute_shdr.Use();
    noise_compute_shdr.SetInt("table_size", table_size);
    noise_compute_shdr.SetInt("noise_side_length", noise_side_length);
    glDispatchCompute(table_size, table_size, table_size);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, table_size, table_size, table_size, 0, GL_RED, GL_FLOAT, 0);

    /*PerlinNoise3D noise;
    noise.Bind();*/

    //////////////////////////////

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
        //volume_shdr.SetFloat("time", current_tick);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS, 0, stars_num);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}