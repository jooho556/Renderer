#version 400 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    float factor = 0.1f;
    vec3 ambient = factor * lightColor;

    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0f);
}