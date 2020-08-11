#include <cassert>
#include <ctime>
#include "Window.h"
#include "Shader.h"
#include "stb_image_write.h"
#include "Constants.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_sdl.h"
#include "IMGUI/imgui_impl_opengl3.h"

bool Window::no_ui = true;

Window::Window(const std::string & title)
    : m_camera(SCREEN_WIDTH, SCREEN_HEIGHT)
{
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    context = SDL_GL_CreateContext(m_window);
    gui.Init(m_window, &context);

    SDL_CaptureMouse(static_cast<SDL_bool>(no_ui));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(no_ui));

    GLenum val = glewInit();
    if (val != GLEW_OK)
    {
        std::cout << "Failed to init glew\n";
        assert(false);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);
    GLSetting();

    srand(unsigned(time(0)));
}

Window::~Window()
{
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
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
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
        {
            terminate = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if(no_ui)
            m_camera.MouseMoveInput(static_cast<float>(event.motion.x), 
                static_cast<float>(event.motion.y));
            break;
        }
        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.scancode)
            {
            case SDL_SCANCODE_F1:
            {
                unsigned char* pixels = new unsigned char[3 * SCREEN_WIDTH * SCREEN_HEIGHT];
                glReadPixels(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);
                stbi_write_png("ScreenShot.png", SCREEN_WIDTH, SCREEN_HEIGHT, 3, &pixels[0], 0);
                delete[] pixels;
                break;
            }                
            case SDL_SCANCODE_GRAVE:
            {
                no_ui = !no_ui;
                SDL_CaptureMouse(static_cast<SDL_bool>(no_ui));
                SDL_SetRelativeMouseMode(static_cast<SDL_bool>(no_ui));
                break;
            }                
            }            
            break;
        }
        }
    }
}

void Window::GLSetting()
{
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
}

void Window::ProcessInput(float dt)
{
    const Uint8 * key_state = SDL_GetKeyboardState(nullptr);
    if (key_state[SDL_SCANCODE_ESCAPE])
        terminate = true;
    if(state != State::GALAXY_DEMO)
        m_camera.KeyboardInput(dt, key_state);
}

void Window::StartDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gui.StartDraw();
    gui.Draw(!no_ui);
}

void Window::EndDraw()
{
    gui.EndDraw();
    SDL_GL_SwapWindow(m_window);
}

void Window::ResetCamera()
{
    m_camera = Camera(SCREEN_WIDTH, SCREEN_HEIGHT);
}

///////////////////////
//GUI Implementation
///////////////////////

Window::GUI::~GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Window::GUI::Init(SDL_Window* window, SDL_GLContext* context)
{
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui::StyleColorsDark();
    const char* glsl_version = "#version 440";
    ImGui_ImplOpenGL3_Init(glsl_version);
    m_window = window;
}

void Window::GUI::StartDraw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_window);
    ImGui::NewFrame();
}

void Window::GUI::Draw(bool draw)
{
    if (!draw) return;
}

void Window::GUI::EndDraw()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
