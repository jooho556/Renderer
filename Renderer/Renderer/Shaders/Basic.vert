#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 oTexCoord;

uniform mat4 model_to_view;
uniform mat4 projection;

void main()
{
     gl_Position = projection * model_to_view * vec4(aPos, 1.f);
     oTexCoord = aTexCoord;
}