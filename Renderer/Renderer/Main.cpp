#include "Window.h"
#include "Shader.h"
#include "Model.h"

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    Shader lighting_shader("Shaders/PongShading.vs", "Shaders/PongShading.fs");
    Shader basic_shader("Shaders/Basic.vs", "Shaders/Basic.fs");
    Model model("Models/sting/Sting-Sword.obj");

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;

        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        model.Draw(lighting_shader, window.GetCamera());
        /////////////////////////////////////////////////////////////////////
        window.EndDraw();

        last_tick = current_tick;
    }

    return 0;
}