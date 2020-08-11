#pragma once
#include "IMGUI/TextEditor.h"
#include "ShaderManager.h"

class ShaderEditor
{
public:
    ShaderEditor();
    void Update();
    ShaderManager& GetShaderManager() { return shdr_mgr; }
    void SetFullScreenShaderMode(bool mode);

private:
    void SetText(const std::string& path);
    void UpdateFileList();
    void RenderText();
    void SaveText(const std::string & text);
    void Compile();
    void AddNewShader();

    ShaderManager shdr_mgr;
    TextEditor editor;
    std::vector<ShaderManager::ShaderInfo> file_info;
    int current_file_index = -1, prev_file_index = -1;
    bool full_screen_shader_mode = false;
    bool new_shader_window = false;
};
