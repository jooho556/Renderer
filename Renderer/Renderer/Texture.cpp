#include <GL/glew.h>
#include <iostream>
#include <cassert>
#include "stb_image.h"
#include "Texture.h"

//Texture::Texture(const std::string & texture_path)
//{
//    LoadTexture(texture_path);
//}
//
//void Texture::LoadTexture(const std::string & texture_path)
//{
//    unsigned int texture_id;
//    glGenTextures(1, &texture_id);
//    texture_units.push_back(texture_id);
//    LoadTexture(texture_path, texture_id);
//}
//
//Texture::~Texture()
//{
//    for (auto & texture_id : texture_units)
//        glDeleteTextures(1, &texture_id);
//}
//
//void Texture::BindTexture() const
//{
//    int texture_num = static_cast<int>(texture_units.size());
//    for (int unit = 0; unit < texture_num; ++unit)
//    {
//        glActiveTexture(GL_TEXTURE0 + unit);
//        glBindTexture(GL_TEXTURE_2D, texture_units[unit]);
//    }
//}
//
//void Texture::UnbindTexture() const
//{
//    int texture_num = static_cast<int>(texture_units.size());
//    for (int unit = 0; unit < texture_num; ++texture_num)
//    {
//        glActiveTexture(GL_TEXTURE0 + unit);
//        glBindTexture(GL_TEXTURE_2D, 0);
//    }
//}

//////////
// TextureBase Implementation
//////////

TextureBase::TextureBase(int texture_type)
    : m_texture_type(texture_type)
{
    glGenTextures(1, &m_id);
}

void TextureBase::BindTexture() const {
    glBindTexture(m_texture_type, m_id);
}

void TextureBase::UnbindTexture() const {
    glBindTexture(m_texture_type, 0);
}

TextureBase::~TextureBase() {
    if (!m_is_copied) glDeleteTextures(1, &m_id);
}


//////////
// Texture1D Implementation
//////////

Texture1D::Texture1D()
    : TextureBase(GL_TEXTURE_1D) {}

Texture1D::Texture1D(const std::string & texture_path)
    : TextureBase(GL_TEXTURE_1D)
{
    LoadTexture(texture_path);
}

void Texture1D::LoadTexture(const std::string & texture_path)
{
    stbi_set_flip_vertically_on_load(true);

    //Load & generate actual texture
    unsigned char * data = stbi_load(texture_path.c_str(),
        &m_width, &m_height, &m_nr_channels, STBI_rgb_alpha);

    if (data)
        LoadTexture(m_width, m_height, GL_RGBA, data);
    else
        std::cout << "ERROR: FAILED TO LOAD TEXTURE\n";

    stbi_image_free(data);
}

void Texture1D::LoadTexture(int width, int height, int format, unsigned char * data)
{
    assert(m_id != -1);
    glBindTexture(GL_TEXTURE_1D, m_id);

    //Texture Wrapping (0 ~ 1)
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Texture filtering - sampling
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage1D(GL_TEXTURE_1D, 0, format,
        width, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_1D);

    glBindTexture(GL_TEXTURE_1D, 0);
}


//////////
// Texture2D Implementation
//////////

Texture2D::Texture2D()
    : TextureBase(GL_TEXTURE_2D) {}

Texture2D::Texture2D(const std::string & texture_path)
    : TextureBase(GL_TEXTURE_2D)
{
    LoadTexture(texture_path);
}

void Texture2D::LoadTexture(const std::string & texture_path)
{
    stbi_set_flip_vertically_on_load(true);

    //Load & generate actual texture
    unsigned char * data = stbi_load(texture_path.c_str(),
        &m_width, &m_height, &m_nr_channels, STBI_rgb_alpha);

    if (data)
        LoadTexture(m_width, m_height, GL_RGBA, data);
    else
        std::cout << "ERROR: FAILED TO LOAD TEXTURE\n";

    stbi_image_free(data);
}

void Texture2D::LoadTexture(int width, int height, int format, unsigned char * data)
{
    assert(m_id != -1);
    glBindTexture(GL_TEXTURE_2D, m_id);

    //Texture Wrapping (0 ~ 1)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Texture filtering - sampling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format,
        width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

 
//////////
// Texture3D Implementation
//////////

Texture3D::Texture3D() : TextureBase(GL_TEXTURE_3D) {}

void Texture3D::LoadTexture(int width, int height, int depth, int format, unsigned char * data)
{
    assert(m_id != -1);
    glBindTexture(GL_TEXTURE_3D, m_id);

    //Texture Wrapping (0 ~ 1)
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    //Texture filtering - sampling
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, format, width, height, depth,
        0, format, GL_FLOAT, data);
    glGenerateMipmap(GL_TEXTURE_3D);

    glBindTexture(GL_TEXTURE_3D, 0);
}
