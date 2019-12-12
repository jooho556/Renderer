#version 440 core

in vec3 Position;
in vec3 Normal;
out vec4 FragColor;

uniform samplerCube skybox;
uniform vec3 camera_pos;

void main()
{
    //Reflection
    vec3 eye_to_frag = normalize(Position - camera_pos);
    vec3 frag_to_cubeface = reflect(eye_to_frag, normalize(Normal));
    vec4 reflected_color = vec4(texture(skybox, frag_to_cubeface).rgb, 1.0);

    //Refraction
    float ratio = 1.f / 1.33f;
    vec3 bended_vector = refract(eye_to_frag, normalize(Normal), ratio);
    vec4 refracted_color = vec4(texture(skybox, bended_vector).rgb, 1.0);

    float t = 0.5f;
    FragColor = t * reflected_color + (1.f - t)* refracted_color;
}