#version 440 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 tex_coord;

void main()
{
    gl_Position = vec4(aPos, 1.f);
    tex_coord = aTexCoord;
}
