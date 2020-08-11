#include <fstream>
#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "Galaxy.h"
#include "PerlinNoise.h"
#include "IMGUI/imgui.h"
#include "ShaderEditor.h"
#include "ShaderManager.h"
#include "EmptyCanvas.h"

void GALAXY_DEMO(const Camera& cam, ShaderEditor& editor);
void NOISE_DEMO(const Camera& cam, ShaderEditor& editor);
void EMPTY_CANVAS(ShaderEditor& editor);

int main(int /*argc*/, char** /*argv*/)
{
    Window window("OpenGL");
    ShaderEditor editor;
    const Camera & cam = window.GetCamera();    

    float last_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
    while (!window.IsDone())
    {
        float current_tick = static_cast<float>(SDL_GetTicks()) * 0.001f;
        float dt = current_tick  - last_tick;
        window.Update(dt);
        window.StartDraw();		            	

        //ImGui::ShowDemoWindow();
            
        ImGui::Begin("Setting", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
        static State state = State::GALAXY_DEMO;
        //ImGui::Combo(std::string("State").c_str(), &state, "Galaxy Demo\0Empty Canvas\0");
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Mode"))
            {
                if (ImGui::MenuItem("Galaxy Demo")) state = State::GALAXY_DEMO;
                if (ImGui::MenuItem("3D Noise Demo")) state = State::NOISE_DEMO;
                //if (ImGui::MenuItem("Full Screen Quad")) state = State::EMPTY_CANVAS;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        window.SetState(static_cast<State>(state));
        switch (window.CurrentState())
        {
        case State::GALAXY_DEMO: 
            window.ResetCamera();
            GALAXY_DEMO(cam, editor); 
            break;
        case State::NOISE_DEMO: NOISE_DEMO(cam, editor); break;
        case State::EMPTY_CANVAS: EMPTY_CANVAS(editor); break;
        }
        ImGui::End();
        window.EndDraw();
        last_tick = current_tick;
    }

    return 0;
}

void GALAXY_DEMO(const Camera& cam, ShaderEditor& editor)
{
    editor.SetFullScreenShaderMode(false);
    static Galaxy galaxy(glm::vec3(0, 0, -40), glm::vec3(0.4f, 0.4, 0.8f), .1f, 2.f);
    Shader* galaxy_shdr = editor.GetShaderManager().GetShader("Stars");
    ComputeShader* galaxy_comp = editor.GetShaderManager().GetComputeShader("Particle");
    static Texture2D particle_tex("Textures/star.png");

    static GalaxyInfo info;
    ImGui::ColorEdit3("Color", &info.col[0]);
    ImGui::ColorEdit3("Core color", &info.core_col[0]);
    ImGui::SliderFloat("Eccentricity", &info.eccentricity, 1.f, 2.f);
    ImGui::SliderFloat("Surface brightness", &info.surface_brightness, 0.1f, 3.f);
    ImGui::SliderInt("Particle number scale", &info.particle_number_scale, 1, 50);
    galaxy.SetCoreColor(info.core_col);
    galaxy.SetColor(info.col);

    if (ImGui::Button("Update"))
        galaxy.Rebuild(info);

    galaxy.Draw(galaxy_comp, galaxy_shdr, cam, &particle_tex);
}

void NOISE_DEMO(const Camera& cam, ShaderEditor& editor)
{
    static ComputeShader* compute = editor.GetShaderManager().GetComputeShader("NoiseGenerator");
    static Shader* volume = editor.GetShaderManager().GetShader("Volume");
    static PerlinNoise noise(*compute);
    noise.Draw(cam, *volume);
}

void EMPTY_CANVAS(ShaderEditor& editor)
{
    editor.SetFullScreenShaderMode(true);
    static EmptyCanvas canvas;
	editor.Update();
    //if(Shader * shader = editor.)
    //canvas.Draw();
}