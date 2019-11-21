#version 440 core

layout(location = 0) in vec4 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;
uniform float tick = 0.05f;

out vec3 out_color;

layout(std430, binding = 1) buffer M
{
    float Mass [];
};

void main()
{
    out_color = color;
    float mass = Mass[gl_VertexID];

    gl_Position = projection * view * model * aPos;
    gl_PointSize = mass / 5.f * (sin(tick + gl_VertexID) + 1);

    float r_sqr = aPos.x * aPos.x + aPos.y * aPos.y;
    if(mass > 29 && r_sqr > 100)
        out_color = vec3(.9f, 0.5f, 0.5f);
}