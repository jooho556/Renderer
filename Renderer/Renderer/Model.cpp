#include <iostream>
#include <GL/glew.h>

#include "stb_image.h"
#include "Model.h"
#include "Shader.h"

void Model::Draw(const Shader & shader, const Camera & cam) const
{
    shader.Use();

    const glm::mat4 & view = cam.GetViewMatrix();
    const glm::mat4 & model = GetModelMat();//glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -3.f));
    const glm::mat4 model_to_view = view * model;

    //////////////////////////////////
    //Vertex shader uniform setting
    shader.SetMat4("model_to_view", model_to_view);
    shader.SetMat4("projection", cam.GetProjectionMatrix());
    //matrix for transforming normal vectors
    shader.SetMat4("normalMat", glm::transpose(glm::inverse(model_to_view)));

    //Directional light property
    shader.SetVec3("dirLight.direction", glm::vec3(-0.2f, -1.f, -0.4f));
    shader.SetVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.SetVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.SetVec3("dirLight.specular", glm::vec3(1.f, 1.f, 1.f));

    //Spot light  property
    shader.SetFloat("spotLight.inner_cutoff", glm::cos(glm::radians(20.f)));
    shader.SetFloat("spotLight.outer_cutoff", glm::cos(glm::radians(25.f)));
    shader.SetFloat("spotLight.constant", 1.f);
    shader.SetFloat("spotLight.linear", 0.09f);
    shader.SetFloat("spotLight.quadratic", 0.032f);
    shader.SetVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.SetVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.SetVec3("spotLight.specular", glm::vec3(1.f, 1.f, 1.f));

    for (unsigned int i = 0; i < meshes.size(); ++i)
        meshes[i].Draw(shader);
}

void Model::LoadModel(const std::string & path)
{
    Assimp::Importer importer;
    const aiScene * scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        ProcessNode(node->mChildren[i], scene);
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    //Process vertex positions, normals, texture coordinates
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;

        glm::vec3 vec;
        vec.x = mesh->mVertices[i].x;
        vec.y = mesh->mVertices[i].y;
        vec.z = mesh->mVertices[i].z;
        vertex.position = vec;

        if (mesh->mNormals)
        {
            vec.x = mesh->mNormals[i].x;
            vec.y = mesh->mNormals[i].y;
            vec.z = mesh->mNormals[i].z;
            vertex.normal = vec;
        }

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texcoord = vec;
        }
        else
            vertex.texcoord = glm::vec2(0, 0);

        vertices.push_back(vertex);
    }

    //Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //Process material
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial * mat, aiTextureType type, const std::string & type_name)
{
    std::vector<Texture> textures;
    unsigned int texture_count = mat->GetTextureCount(type);
    for (unsigned int i = 0; i < texture_count; ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        unsigned int texture_loaded_size = textures_loaded.size();
        for (unsigned int j = 0; j < texture_loaded_size; ++j)
        {
            if (textures_loaded[j].path == std::string(str.C_Str()))
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = type_name;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }        
    }
    return textures;
}

unsigned int Model::TextureFromFile(const std::string & name, const std::string & directory)
{
    std::string filename = directory + '/' + name;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char * data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    }
    else
        std::cout << "Texture failed to load at path : " << name << std::endl;

    stbi_image_free(data);
    return textureID;
}
