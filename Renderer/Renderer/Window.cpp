#include <cassert>
#include "Window.h"
#include "Shader.h"
#include "Texture.h"

namespace
{
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 600;
}

Window::Window(const std::string & title)
    : m_camera(WINDOW_WIDTH, WINDOW_HEIGHT)
{
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(m_window);

    SDL_CaptureMouse(SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    GLenum val = glewInit();
    if (val != GLEW_OK)
    {
        std::cout << "Failed to init glew\n";
        assert(false);
    }
    glClearColor(0.f, 0.2f, 0.f, 1.f);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
}

Window::~Window()
{
    Destroy();
}

void Window::Update(float dt)
{
    PollEvent();
    ProcessInput(dt);
}

void Window::PollEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            terminate = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            m_camera.MouseMoveInput(event.motion.x, event.motion.y);
            break;
        }
        }
    }
}

void Window::ProcessInput(float dt)
{
    const Uint8 * state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_ESCAPE])
        terminate = true;
    m_camera.KeyboardInput(dt, state);
}

void Window::StartDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Window::EndDraw()
{
    SDL_GL_SwapWindow(m_window);
}

void Window::Draw(const Drawable & drawable)
{
    Shader * shader = drawable.GetShader();
    shader->Use();

    shader->SetMat4("view", m_camera.GetViewMatrix());
    shader->SetMat4("projection", m_camera.GetProjectionMatrix());
    shader->SetMat4("model", drawable.GetModel());
    shader->SetVec3("objectColor", drawable.GetColor());

    drawable.Bind();

    if (Texture * texture = drawable.GetTexture())
        texture->BindTexture();

    glDrawArrays(GL_TRIANGLES, 0, drawable.GetVertexNum());
    //glDrawElements(GL_TRIANGLES, drawable.GetElementNum(), GL_UNSIGNED_INT, 0);

    drawable.Unbind();
}

void Window::DrawLighting(const Drawable & drawable, const Drawable * light)
{
    Shader * shader = drawable.GetShader();
    shader->Use();

    const glm::mat4 & view = m_camera.GetViewMatrix();
    const glm::mat4 & model = drawable.GetModel();
    const glm::mat4 model_to_view = view * model;

    shader->SetMat4("model_to_view", model_to_view);
    shader->SetMat4("projection", m_camera.GetProjectionMatrix());
    //matrix for transforming normal vectors
    shader->SetMat4("normalMat", glm::transpose(glm::inverse(model_to_view)));

    shader->SetVec3("objectColor", drawable.GetColor());
    shader->SetVec3("lightColor", light->GetColor());
    shader->SetVec3("lightPos", view * glm::vec4(light->GetPosition(), 1.f));
    drawable.Bind();

    if (Texture * texture = drawable.GetTexture())
        texture->BindTexture();

    glDrawArrays(GL_TRIANGLES, 0, drawable.GetVertexNum());
    //glDrawElements(GL_TRIANGLES, drawable.GetElementNum(), GL_UNSIGNED_INT, 0);

    drawable.Unbind();
}

void Window::Destroy()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
