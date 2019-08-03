#include "Window.h"
#include "Shader.h"
#include "Model.h"

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    Shader lighting_shader("Shaders/PongShading.vs", "Shaders/PongShading.fs");
    Shader basic_shader("Shaders/Basic.vs", "Shaders/Basic.fs");
    Shader depth_shader("Shaders/Basic.vs", "Shaders/DepthTest.fs");
    Model model("Models/suit/scene.gltf");
    model.SetScale(0.5f);

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;

        window.Update(dt);
        window.StartDraw();
        /////////////////////////////////////////////////////////////////////
        model.SetPosition(glm::vec3(2, -2, -3));
        model.Draw(lighting_shader, window.GetCamera());
        model.SetPosition(glm::vec3(2, -2, -6));
        model.Draw(lighting_shader, window.GetCamera());
        model.SetPosition(glm::vec3(-2, -2, -3));
        model.Draw(lighting_shader, window.GetCamera());
        model.SetPosition(glm::vec3(-2, -2, -6));
        model.Draw(lighting_shader, window.GetCamera());
        /////////////////////////////////////////////////////////////////////
        window.EndDraw();

        last_tick = current_tick;
    }

    return 0;
}