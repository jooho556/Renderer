#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aTexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 eye_pos;

out vec3 o_texcoord;
out vec3 ray_dir;

void main()
{    
    o_texcoord = aTexCoord;
    ray_dir = aPos - eye_pos;

    gl_Position = projection * view * vec4(aPos, 1.f);
}