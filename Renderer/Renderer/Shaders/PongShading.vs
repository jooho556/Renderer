#version 400 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model_to_view;
uniform mat4 projection;
uniform mat4 normalMat;

void main()
{
     vec4 pos_viewcoord =  model_to_view * vec4(aPos, 1.f);
     gl_Position = projection * pos_viewcoord;
     Normal = mat3(normalMat) * aNormal;
     FragPos = vec3(pos_viewcoord);
}
