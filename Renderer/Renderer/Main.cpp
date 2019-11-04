#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"

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

    int particle_num = 1000000;
    std::vector<float> particle_init_pos;
    for (int z = -50; z < 50; ++z)
    {
        for (int y = -50; y < 50; ++y)
        {
            for (int x = -50; x < 50; ++x)
            {
                particle_init_pos.push_back(x / 1000.f);
                particle_init_pos.push_back(y / 1000.f);
                particle_init_pos.push_back(z / 1000.f);
                particle_init_pos.push_back(1.f);
            }
        }
    }
    

    unsigned int particle_vao;
    glGenVertexArrays(1, &particle_vao);
    glBindVertexArray(particle_vao);

    int particle_buf_size = particle_num * sizeof(float) * 4;
    unsigned int particle_pos_buf;
    glGenBuffers(1, &particle_pos_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particle_pos_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_buf_size, &particle_init_pos[0], GL_DYNAMIC_DRAW);

    std::vector<float> particle_init_vel(particle_buf_size, 0.f);
    unsigned int particle_vel_buf;
    glGenBuffers(1, &particle_vel_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particle_vel_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_buf_size, &particle_init_vel[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, particle_pos_buf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    Shader particle_compute_shader("Shaders/Particle.cs");
    Shader particle_shader("Shaders/Particle.vs", "Shaders/Particle.fs");

    unsigned int bhVao;
    glGenVertexArrays(1, &bhVao);
    glBindVertexArray(bhVao);

    unsigned int bhBuf;
    glGenBuffers(1, &bhBuf);
    glBindBuffer(GL_ARRAY_BUFFER, bhBuf);
    float data[] = { -5, 0, 0, 1, 5, 0, 0, 1 };
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), data, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    /*****************************************************/
    /*****************************************************/

    Skybox space(space_cube);
    //Model model("Models/suit/scene.gltf");

    const Camera & cam = window.GetCamera();

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;

        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////

        particle_compute_shader.Use();
        glm::vec3 att1_pos = glm::vec3(std::cos(SDL_GetTicks() * 0.0001f), std::sin(SDL_GetTicks() * 0.0001f), 0);
        glm::vec3 att2_pos = glm::vec3(-std::cos(SDL_GetTicks() * 0.0001f), -std::sin(SDL_GetTicks() * 0.0001f), 0);
        particle_compute_shader.SetVec3("BlackHolePos1", att1_pos);
        particle_compute_shader.SetVec3("BlackHolePos2", att2_pos);
        glDispatchCompute(particle_num / 1000, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        particle_shader.Use();
        particle_shader.SetMat4("model_to_view", cam.GetViewMatrix());
        particle_shader.SetMat4("projection", cam.GetProjectionMatrix());
        particle_shader.SetVec3("color", glm::vec3(0.9f, 0.9f, 0.5f));
        //particle_compute_shader.SetFloat("Gravity1", 1000);
        //particle_compute_shader.SetFloat("Gravity2", 1000);
        //particle_compute_shader.SetFloat("ParticleMass", 0.1);
        //particle_compute_shader.SetFloat("ParticleInvMass", 100);
        //particle_compute_shader.SetFloat("dt", 0.0005);
        //particle_compute_shader.SetFloat("MaxDist", 10);

        //space.Draw(cubemap_shader, cam);
        //model.Draw(lighting_shader, cam);

        glPointSize(1.f);
        glBindVertexArray(particle_vao);
        glDrawArrays(GL_POINTS, 0, particle_num);
        glBindVertexArray(0);

        particle_shader.SetVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
        glPointSize(10.f);
        float att_data[] = { att1_pos.x, att1_pos.y, att1_pos.z, 1.f, att2_pos.x, att2_pos.y, att2_pos.z, 1.f };
        glBindBuffer(GL_ARRAY_BUFFER, bhBuf);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(float), att_data);
        glBindVertexArray(bhVao);
        glDrawArrays(GL_POINTS, 0, 2);
        glBindVertexArray(0);

        /////////////////////////////////////////////////////////////////////
        window.EndDraw();

        last_tick = current_tick;
    }

    glDeleteVertexArrays(1, &particle_vao);
    glDeleteBuffers(1, &particle_pos_buf);
    glDeleteBuffers(1, &particle_vel_buf);
    return 0;
}