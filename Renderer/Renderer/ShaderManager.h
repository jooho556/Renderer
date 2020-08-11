#pragma once
#include <unordered_map>
#include <memory>
#include "Shader.h"

class ShaderManager
{
public:
    ShaderManager();

    struct ShaderInfo
    {
        ShaderInfo(const std::string& p, const std::string& n, const std::string& e)
            : path(p), name(n), extension(e) {}

        bool operator==(const ShaderInfo& info)
        {
            return info.path == path && info.name == name && info.extension == extension;
        }

        std::string path;
        std::string name;
        std::string extension;
    };

    Shader* GetShader(const std::string& name, bool full_screen_shader_mode = false);   
    ComputeShader* GetComputeShader(const std::string& name);    
    ShaderBase* GetShader(const std::string& name, const std::string& extension, bool full_screen_shader_mode);
    ShaderInfo AddNewShader(int type, const std::string& name);

private:
    enum Type {
        VERT, FRAG, COMP, FRAG_FS
    };

    std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
    std::unordered_map<std::string, std::unique_ptr<ComputeShader>> comp_shaders;
    std::unordered_map<std::string, std::unique_ptr<Shader>> fullscreen_shaders;
};