#pragma once
#include <vector>
#include <string>

namespace Old
{
    class Texture
    {
    public:
        Texture() {}
        Texture(const std::string & texture_path);
        Texture(const std::vector<std::string> & texture_paths);
        ~Texture();

        void LoadTexture(const std::string& texture_path);
        void BindTexture() const;
        void UnbindTexture() const;

        //unsigned int m_textureID = 0;

    private:
        void LoadTexture(const std::string& texture_path, unsigned int textureID);

        int m_width = 0;
        int m_height = 0;
        int m_nr_channels = 0;

        std::vector<unsigned int> texture_units;
    };
}
