#include <fstream>
#include <filesystem>
#include <algorithm>
#include "ShaderEditor.h"

ShaderEditor::ShaderEditor()
{
    auto lang = TextEditor::LanguageDefinition::GLSL();
    editor.SetLanguageDefinition(lang);
    UpdateFileList();
}

void ShaderEditor::Update()
{    
	if (new_shader_window) AddNewShader();

	std::vector<const char*> cstrs;
    for (auto& str : file_info)
        cstrs.push_back(&str.path.front());        
    ImGui::Combo("Shader", &current_file_index, cstrs.data(), static_cast<int>(file_info.size()));

    if (current_file_index != prev_file_index)
    {
        SetText(file_info[current_file_index].path);
		prev_file_index = current_file_index;
    }

	if (current_file_index != -1)
		RenderText();
}

void ShaderEditor::SetFullScreenShaderMode(bool mode)
{
	if (full_screen_shader_mode == mode)
		return;

	full_screen_shader_mode = mode;
	UpdateFileList();
}

void ShaderEditor::RenderText()
{
	auto cpos = editor.GetCursorPosition();
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))							
				SaveText(editor.GetText());			
			if (ImGui::MenuItem("Save & Compile"))
			{
				SaveText(editor.GetText());
				Compile();
			}
			if (ImGui::MenuItem("Add"))
				new_shader_window = true;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ShaderManager::ShaderInfo info = file_info[current_file_index];
	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(), info.path.c_str());

	editor.Render("TextEditor");

	//Error printing
	ImGui::Begin("Console");
	if (ShaderBase* shdr = shdr_mgr.GetShader(info.name, info.extension, full_screen_shader_mode))
	{
		const std::string& error_log = shdr->GetErrorLog();
		if (!error_log.empty())
			ImGui::TextWrapped(error_log.c_str());
	}
	else
		ImGui::TextWrapped("Cannot compile - Current shader is null\n");
	
	ImGui::End();
}

void ShaderEditor::SaveText(const std::string& text)
{
	std::ofstream save_file(file_info[current_file_index].path.c_str());
	if (save_file.is_open())	
		save_file << text;	
}

void ShaderEditor::Compile()
{
	ShaderManager::ShaderInfo info = file_info[current_file_index];
	if(ShaderBase* shdr = shdr_mgr.GetShader(info.name, info.extension, full_screen_shader_mode))
		shdr->Compile();
}

void ShaderEditor::AddNewShader()
{
	ImGui::Begin("New shader");
	const char* items[] = { "Vertex Shader", "Fragment Shader",
		"Compute Shader", "Fragment Shader (Full screen shader)" };
	static int current = 0;
	ImGui::Combo("Type", &current, items, 4);

	static char name[32] = "Empty";
	ImGui::InputText("Name", name, 32);

	if (ImGui::Button("Add"))
	{
		ShaderManager::ShaderInfo new_info= shdr_mgr.AddNewShader(current, name);
		UpdateFileList();
		auto it = std::find(file_info.begin(), file_info.end(), new_info);
		int index = static_cast<int>(it - file_info.begin());
		current_file_index = (--index) < 0 ? 0 : index;
		new_shader_window = false;
	}

	ImGui::End();
}

void ShaderEditor::SetText(const std::string& path)
{
    std::ifstream file(path);
    if (file.is_open())
    {
        std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        editor.SetText(text);
    }
    file.close();
}

void ShaderEditor::UpdateFileList()
{
    file_info.clear();
    std::string base = full_screen_shader_mode ? "Shaders\\FullScreenShaders" : "Shaders";
	for (const auto& entry : std::filesystem::directory_iterator(base))
		if(!entry.is_directory())
			file_info.emplace_back(entry.path().string(), 
				entry.path().stem().string(),
				entry.path().extension().string());
}
