#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_scancode.h"

class Camera
{
public:
    Camera(unsigned int window_width, unsigned int window_height,
        const glm::vec3 & pos = glm::vec3(0.f, 0.f, 0.f), const glm::vec3 & front = glm::vec3(0, 0, -1.f))
    : m_projection(glm::perspective(glm::radians(90.f), (float)window_width / (float)window_height, 0.1f, 1000.f)),
        m_pos(pos), 
        m_last_x(static_cast<float>(window_width / 2)), 
        m_last_y(static_cast<float>(window_height / 2)),
        m_front(front)
    {
        if (m_front == glm::vec3(0, 1, 0))
            m_up = glm::vec3(0, 0, -1);
        else if (m_front == glm::vec3(0, -1, 0))
            m_up = glm::vec3(0, 0, 1);
        UpdateView();
    }

    void MoveBy(const glm::vec3 & vec)
    { 
        m_pos += vec; 
        UpdateView();
    }
    void MoveBy(float x, float y, float z) { MoveBy(glm::vec3(x, y, z)); }
    void SetPosition(const glm::vec3 & vec)
    { 
        m_pos = vec; 
        UpdateView();
    }
    void SetPosition(float x, float y, float z) { SetPosition(glm::vec3(x, y, z)); }

    const glm::vec3 & GetPosition() const { return m_pos; }
    const glm::mat4 & GetViewMatrix() const { return m_view; }
    const glm::mat4 & GetProjectionMatrix() const { return m_projection; }

    void KeyboardInput(float dt, const Uint8 * state)
    {
        float move_speed = 55.f * dt;

        if(state[SDL_SCANCODE_W])
            m_pos += move_speed * m_front;
        if (state[SDL_SCANCODE_S])
            m_pos -= move_speed * m_front;
        if (state[SDL_SCANCODE_A])
            m_pos -= glm::normalize(glm::cross(m_front, m_up)) * move_speed;
        if (state[SDL_SCANCODE_D])
            m_pos += glm::normalize(glm::cross(m_front, m_up)) * move_speed;

        UpdateView();
    }

    void MouseMoveInput(float x, float y)
    {
        float xoffset = x - m_last_x;
        float yoffset = m_last_y - y;
        m_last_x = x;
        m_last_y = y;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        if (m_pitch > 89.f)
            m_pitch = 89.f;
        else if (m_pitch < -89.f)
            m_pitch = -89.f;

        glm::vec3 front;
        front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
        front.y = sin(glm::radians(m_pitch));
        front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
        m_front = glm::normalize(front);
    }

private:
    void UpdateView()
    {
        m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);
    }
    glm::mat4 m_view = glm::mat4(1.f);
    glm::mat4 m_projection = glm::mat4(1.f);

    glm::vec3 m_pos = glm::vec3(0.f, 0.f, 3.f);
    glm::vec3 m_front = glm::vec3(0.f, 0.f, -1.f);
    glm::vec3 m_up = glm::vec3(0.f, 1.f, 0.f);

    float m_last_x = 0.f;
    float m_last_y = 0.f;
    float m_yaw = -90.f;
    float m_pitch = 0.f;
    float m_fov = 45.f;
};