#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include "Drawable.h"
#include "Camera.h"

class Window
{
public:
    Window(const std::string & title);
    ~Window();

    void Update(float dt);

    void StartDraw();
    void EndDraw();
    void Draw(const Drawable & drawable);
    void DrawLighting(const Drawable & drawable, const Drawable * light);

    bool IsDone() const { return terminate; }

private:
    void Destroy();

    void ProcessInput(float dt);
    void PollEvent();

    Camera m_camera;
    SDL_Window * m_window;
    SDL_GLContext context;
    bool terminate = false;
};