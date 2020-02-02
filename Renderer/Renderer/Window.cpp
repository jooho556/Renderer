#include <cassert>
#include <ctime>
#include "Window.h"
#include "Shader.h"
#include "stb_image_write.h"

namespace
{
    constexpr int WINDOW_WIDTH = 1920;
    constexpr int WINDOW_HEIGHT = 1080;
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_GL_SetSwapInterval(1);

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
  
    glClearColor(0.05f, 0.f, 0.1f, 1.f);

    srand(unsigned(time(0)));
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
            m_camera.MouseMoveInput(static_cast<float>(event.motion.x), 
                static_cast<float>(event.motion.y));
            break;
        }
        case SDL_KEYDOWN:
        {
            if (event.key.keysym.scancode == SDL_SCANCODE_F1)
            {
                unsigned char * pixels = new unsigned char[3 * WINDOW_WIDTH * WINDOW_HEIGHT];
                glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                stbi_write_png("ScreenShot.png", WINDOW_WIDTH, WINDOW_HEIGHT, 3, &pixels[0], 0);
                delete[] pixels;
            }
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

void Window::Destroy()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
