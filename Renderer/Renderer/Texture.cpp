#include <GL/glew.h>
#include <iostream>
#include "stb_image.h"
#include "Texture.h"

Texture::Texture()
{
    glGenTextures(1, &m_textureID);
}

Texture::Texture(const std::string & texture_path)
{
    glGenTextures(1, &m_textureID);
    LoadTexture(texture_path);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

void Texture::LoadTexture(const std::string & texture_path)
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);

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

void Texture::BindTexture() const
{
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::UnbindTexture() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
