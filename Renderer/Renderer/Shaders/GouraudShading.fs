#version 400 core

out vec4 FragColor;
in vec4 finalColor;

void main()
{
    FragColor = finalColor;
}
