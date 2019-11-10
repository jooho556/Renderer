#include <GL/glew.h>
#include <iostream>
#include "stb_image.h"
#include "Texture.h"

namespace Old
{
    //Normal Ctor
    Texture::Texture(const std::string & texture_path)
    {
        LoadTexture(texture_path);
    }

    //When using multiple texture units
    Texture::Texture(const std::vector<std::string>& texture_paths)
    {
        for (auto & path : texture_paths)
            LoadTexture(path);
    }

    void Texture::LoadTexture(const std::string & texture_path)
    {
        unsigned int texture_id;
        glGenTextures(1, &texture_id);
        texture_units.push_back(texture_id);
        LoadTexture(texture_path, texture_id);
    }

    Texture::~Texture()
    {
        for (auto & texture_id : texture_units)
            glDeleteTextures(1, &texture_id);
    }

    void Texture::BindTexture() const
    {
        int texture_num = texture_units.size();
        for (int unit = 0; unit < texture_num; ++unit)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, texture_units[unit]);
        }
    }

    void Texture::UnbindTexture() const
    {
        int texture_num = texture_units.size();
        for (int unit = 0; unit < texture_num; ++texture_num)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    void Texture::LoadTexture(const std::string & texture_path, unsigned int textureID)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);

        //Texture Wrapping (0 ~ 1)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //Texture filtering - sampling
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
        //Load & generate actual texture
        unsigned char * data = stbi_load(texture_path.c_str(),
            &m_width, &m_height, &m_nr_channels, STBI_rgb_alpha);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
            std::cout << "ERROR: FAILED TO LOAD TEXTURE\n";

        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
