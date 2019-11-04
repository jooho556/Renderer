#version 440 core

layout(location = 0) in vec3 aPosition;

out vec3 TexCoords;
uniform mat4 model_to_projection;

void main()
{
    TexCoords = aPosition;
    vec4 pos = model_to_projection * vec4(aPosition, 1.0);
    gl_Position = pos.xyww;
}