#version 400 core

out vec4 FragColor;
in vec2 oTexCoord;

uniform vec3 objectColor;
//uniform sampler2D ourTexture;

void main()
{
    //FragColor =  texture(ourTexture, oTexCoord);
    FragColor = vec4(objectColor, 1.f);
}