#version 440 core

layout(location = 0) in vec4 aPos;

uniform mat4 model_to_view;
uniform mat4 projection;

void main()
{
     gl_Position = projection * model_to_view * aPos;
}