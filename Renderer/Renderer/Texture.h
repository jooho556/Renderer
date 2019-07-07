#pragma once

#include <string>

class Texture
{
public:
    Texture();
    Texture(const std::string & texture_path);
    ~Texture();

    void LoadTexture(const std::string& texture_path);
    void BindTexture() const;
    void UnbindTexture() const;

    unsigned int m_textureID = 0;

private:
    int m_width = 0;
    int m_height = 0;
    int m_nr_channels = 0;
};