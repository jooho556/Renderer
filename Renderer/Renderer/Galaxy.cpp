#include <vector>
#include "Shader.h"
#include "Ellipse.h"
#include "GL/glew.h"
#include "Galaxy.h"
#include "Camera.h"

int Galaxy::galaxy_num = 0;
int Galaxy::particle_num = 0;

Galaxy::Galaxy(const glm::vec3 & pos, const glm::vec3 & nebula_color, const glm::vec3 & star_color)
    : pos(pos), nebula_color(nebula_color), star_color(star_color)
{
    galaxy_num++;
    if (galaxy_num == 1)
        CreateGalaxy();
}

Galaxy::~Galaxy()
{
    galaxy_num--;
    if (galaxy_num == 0)
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &pos_buf);
        glDeleteBuffers(1, &mass_buf);
        glDeleteBuffers(1, &e_info_buf);
        glDeleteBuffers(1, &e_rinfo_buf);
    }
}

void Galaxy::Draw(Shader * compute, Shader * nebula, Shader * star, 
    const Camera * cam, const Old::Texture * tex)
{
    glBindVertexArray(vao);
    tex->BindTexture();

    //Update position
    compute->Use();
    compute->SetFloat("PI", Angle::PI);
    glDispatchCompute(particle_num / 1000 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //Draw nebula
    nebula->Use();
    nebula->SetMat4("model", glm::translate(pos));
    nebula->SetMat4("view", cam->GetViewMatrix());
    nebula->SetMat4("projection", cam->GetProjectionMatrix());
    nebula->SetVec3("color", nebula_color);
    glDrawArrays(GL_POINTS, 0, particle_num);

    //Draw star
    star->Use();
    star->SetMat4("model", glm::translate(pos));
    star->SetMat4("view", cam->GetViewMatrix());
    star->SetMat4("projection", cam->GetProjectionMatrix());
    star->SetVec3("color", star_color);
    glDrawArrays(GL_POINTS, 0, particle_num);

    glBindVertexArray(0);
}

void Galaxy::CreateGalaxy()
{
    std::vector<float> particle_init_pos;
    std::vector<float> ellipse_info;
    std::vector<glm::mat4> rot_info;

    Ellipse ellipse(0.2f, 1.25f);
    for (float rotate_angle = 0.f; rotate_angle < Angle::PI * 1.5f; rotate_angle += 0.016f)
    {
        ellipse.Rotate(rotate_angle);
        int point_num = static_cast<int>(deVaucouleurs(ellipse.GetRadA()) * 7);
        particle_num += point_num;
        for (int i = 0; i < point_num; ++i)
        {
            float t = rand() % 1000 / 1000.f;
            glm::vec3 point = ellipse.GetPoint(t);
            particle_init_pos.push_back(point.x);
            particle_init_pos.push_back(point.y);
            particle_init_pos.push_back(0);
            particle_init_pos.push_back(1.f);

            ellipse_info.push_back(t);
            ellipse_info.push_back(ellipse.GetRadA());
            ellipse_info.push_back(ellipse.GetRadB());
            ellipse_info.push_back(0);
            rot_info.push_back(ellipse.GetRotationMat());
        }
        ellipse.AddRadius(0.1f);
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Position
    int particle_buf_size = particle_num * sizeof(float) * 4;
    glGenBuffers(1, &pos_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pos_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_buf_size, &particle_init_pos[0], GL_DYNAMIC_DRAW);

    //Mass
    std::vector<float> particle_mass_vec;
    for (int i = 0; i < particle_num; ++i)
        particle_mass_vec.push_back(static_cast<float>(rand() % 30 + 1));

    glGenBuffers(1, &mass_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mass_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, particle_num * sizeof(float), &particle_mass_vec[0], GL_DYNAMIC_DRAW);

    //ellipse info for calculating velocity
    glGenBuffers(1, &e_info_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, e_info_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ellipse_info.size() * sizeof(float), &ellipse_info[0], GL_DYNAMIC_DRAW);

    //ellipse rotation info
    glGenBuffers(1, &e_rinfo_buf);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, e_rinfo_buf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, rot_info.size() * sizeof(glm::mat4), &rot_info[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, pos_buf);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

float Galaxy::deVaucouleurs(float radius, float scale_length)
{
    float i0 = 1.f;
    return static_cast<float>(i0 * std::exp(-7.669 * (powf(radius / scale_length, 0.25f) - 1)));
}