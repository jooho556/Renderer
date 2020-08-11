#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include "Camera.h"

enum class State {
    GALAXY_DEMO,
    NOISE_DEMO,
    EMPTY_CANVAS,
    COUNT
};

class Window
{
public:
    Window(const std::string & title);
    ~Window();

    void Update(float dt);

    void StartDraw();
    void EndDraw();
    bool IsDone() const { return terminate; }
    const Camera & GetCamera() const { return m_camera; }
    void ResetCamera();
    State CurrentState() const { return state; }
    State& CurrentState() { return state; }
    void SetState(State new_state) { state = new_state; }
    static bool NoUI() { return no_ui; }

private:
    class GUI
    {
    public:
        GUI() {}
        ~GUI();
        void Init(SDL_Window* window, SDL_GLContext* context);

        void StartDraw();
        void Draw(bool draw);
        void EndDraw();

    private:
        SDL_Window* m_window;
    };

    void ProcessInput(float dt);
    void PollEvent();
    void GLSetting();

    Camera m_camera;
    SDL_Window * m_window;
    SDL_GLContext context;
    GUI gui;
    State state = State::GALAXY_DEMO;
    bool terminate = false;
    static bool no_ui;
};
