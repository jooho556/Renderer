#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model_to_view;
uniform mat4 normalMat;
uniform mat4 projection;

void main()
{
    Normal = normalMat * aNormal;
    Position = vec3(model_to_view * vec4(aPos, 1.0));
    gl_Position = projection * vec4Position, 1.0);
}