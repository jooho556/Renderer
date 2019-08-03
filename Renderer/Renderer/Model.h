#pragma once
#include <string>
#include <vector>
#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include "Mesh.h"
#include "Camera.h"

class Model
{
public:
    Model(const std::string & path)
    {
        LoadModel(path);
    }

    void Draw(const Shader & shader, const Camera & cam) const;

    const glm::vec3 & GetPosition() const { return position; }
    void SetPosition(const glm::vec3 & pos) { position = pos; }

    float GetScale() const { return scale; }
    void SetScale(float s) { scale = s; }

private:
    void LoadModel(const std::string & path);
    void ProcessNode(aiNode * node, const aiScene * scene);
    Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial * mat, aiTextureType type, const std::string & type_name);
    unsigned int TextureFromFile(const std::string & name, const std::string & directory);
    glm::mat4 GetModelMat() const
    {
        return glm::scale(glm::translate(glm::mat4(1.f), position), glm::vec3(scale, scale, scale));
    }

    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;
    glm::vec3 position;
    float scale;
};