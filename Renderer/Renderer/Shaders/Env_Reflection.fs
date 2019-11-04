#version 440 core

in vec3 Position;
in vec3 Normal;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    vec3 eye_to_frag = normalize(Position);
    vec3 frag_to_cubeface = reflect(eye_to_frag, normalize(Normal));
    FragColor = vec4(texxture(skybox, frag_to_cubeface).rgb, 1.0);
}