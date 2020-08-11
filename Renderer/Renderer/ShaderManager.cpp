#include <filesystem>
#include <iostream>
#include <fstream>
#include "ShaderManager.h"

namespace
{
	std::string basecode = "#version 440 core\n"
			"void main()\n"
			"{\n"
			"}\n";

	std::string frag_fs_basecode = "#version 440 core\n"
		"out vec4 frag_color;\n"
		"void main()\n"
		"{\n"
		"	frag_color = vec4(1, 1, 1, 1);\n"
		"}\n";
}

ShaderManager::ShaderManager()
{
	std::string base("Shaders");
	for (const auto& entry : std::filesystem::directory_iterator(base))
	{
		std::string path = entry.path().string(); // Shaders/Basic.frag
		std::string stem = entry.path().stem().string(); // Basic
		//std::string filename = entry.path().filename().string(); // Basic.frag
		std::string extension = entry.path().extension().string(); // .frag

		if (extension == ".vert")
			shaders[stem] = std::make_unique<Shader>(path, base + std::string("\\") + stem + std::string(".frag"));
		else if (extension == ".comp")		
			comp_shaders[stem] = std::make_unique<ComputeShader>(path);
	}

	base = "Shaders\\FullScreenShaders";
	for (const auto& entry : std::filesystem::directory_iterator(base))
	{
		std::string path = entry.path().string(); // Shaders/Basic.frag
		std::string stem = entry.path().stem().string(); // Basic
		//std::string filename = entry.path().filename().string(); // Basic.frag
		std::string extension = entry.path().extension().string(); // .frag

		if (extension == ".frag")
			shaders[stem] = std::make_unique<Shader>(base + std::string("\\") + stem + std::string(".vert"), path);
	}
}

Shader* ShaderManager::GetShader(const std::string& name, bool full_screen_shader_mode)
{
	auto& container = full_screen_shader_mode ? fullscreen_shaders : shaders;
	auto shdr = container.find(name);
	if (shdr == container.end()) return nullptr;
	return &(*(shdr->second));
}

ComputeShader* ShaderManager::GetComputeShader(const std::string& name)
{
	auto shdr = comp_shaders.find(name);
	if (shdr == comp_shaders.end()) return nullptr;
	return &(*(shdr->second));
}

ShaderBase* ShaderManager::GetShader(const std::string& name, const std::string& extension, bool full_screen_shader_mode)
{
	if (extension != ".vert" && extension != ".frag" && extension != ".comp") return nullptr;
	if (extension == ".comp")
		return GetComputeShader(name);
	else
		return GetShader(name, full_screen_shader_mode);
}

ShaderManager::ShaderInfo ShaderManager::AddNewShader(int t, const std::string& name)
{
	Type type = static_cast<Type>(t);
	std::string base;

	if (type != Type::FRAG_FS)
		base = "Shaders";
	else
		base = "Shaders\\FullScreenShaders";

	std::string extension;
	switch (type)
	{
	case Type::VERT: extension = ".vert"; break;	
	case Type::COMP: extension = ".comp"; break;
	case Type::FRAG:
	case Type::FRAG_FS: extension = ".frag"; break;
	}

	std::string path = (base + std::string("\\") + name + extension);
	std::ofstream file(path);
	if (file.is_open())	
		file << ((type == Type::FRAG_FS) ? frag_fs_basecode : basecode);	
	file.close();

	switch (type)
	{
	case Type::VERT: 
	case Type::FRAG:
	{
		auto shdr = shaders.find(name);
		if (shdr == shaders.end())
			shaders[name] = std::make_unique<Shader>(
				Shader((base + std::string("\\") + name + std::string(".vert")),
					(base + std::string("\\") + name + std::string(".frag"))));
		else
			shdr->second->Compile();
		break;
	}	
	case Type::COMP: 
	{
		auto shdr = comp_shaders.find(name);
		if (shdr == comp_shaders.end())
			comp_shaders[name] = std::make_unique<ComputeShader>(ComputeShader(path));
		else
			shdr->second->Compile();
		break;
	}		
	case Type::FRAG_FS: 
	{
		auto shdr = fullscreen_shaders.find(name);
		if (shdr == fullscreen_shaders.end())
			fullscreen_shaders[name] = std::make_unique<Shader>(Shader((base + std::string("\\") + std::string("FSQ.vert")), path));
		else
			shdr->second->Compile();
		break;
	}		
	}
	return ShaderInfo(path, name, extension);
}
