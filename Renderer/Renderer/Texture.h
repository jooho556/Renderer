#pragma once
#include <vector>
#include <string>
#include "GLObject.h"

class TextureBase : public GLObject
{
public:
    TextureBase(int texture_type);

    TextureBase(const TextureBase & tb)
        : GLObject(tb), m_texture_type(tb.m_texture_type),
        m_width(tb.m_width), m_height(tb.m_height), m_nr_channels(tb.m_nr_channels) {}

    TextureBase(TextureBase && tb)
        : GLObject(tb), m_texture_type(tb.m_texture_type),
        m_width(tb.m_width), m_height(tb.m_height), m_nr_channels(tb.m_nr_channels) {}

    virtual ~TextureBase() override;

    void BindTexture() const;
    void UnbindTexture() const;
    
    virtual void LoadTexture(const std::string & texture_path) = 0;

protected:
    int m_texture_type = -1;
    int m_width = 0;
    int m_height = 0;
    int m_nr_channels = 0;
};

class Texture1D : public TextureBase
{
public:
    Texture1D();
    Texture1D(const std::string & texture_path);

    virtual void LoadTexture(const std::string& texture_path) override;
    void LoadTexture(int width, int height, int format, unsigned char * data = nullptr);
};

class Texture2D : public TextureBase
{
public:
    Texture2D();
    Texture2D(const std::string & texture_path);

    virtual void LoadTexture(const std::string& texture_path) override;
    void LoadTexture(int width, int height, int format, unsigned char * data = nullptr);
};

class Texture3D : public TextureBase
{
public:
    Texture3D();
    void LoadTexture(int width, int height, int depth, int format, unsigned char * data = nullptr);

private:
    virtual void LoadTexture(const std::string& texture_path) override {}
    int m_depth = 0;
};

