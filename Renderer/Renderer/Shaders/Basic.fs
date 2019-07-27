#version 400 core

out vec4 FragColor;
in vec2 oTexCoord;

//uniform vec3 objectColor;
uniform sampler2D texture_diffuse1;

void main()
{
    FragColor =  texture(texture_diffuse1, oTexCoord);
}