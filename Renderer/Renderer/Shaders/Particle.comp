#version 440 core

uniform float PI;
uniform float dt = 0.0005f;

layout(local_size_x = 1000) in;

struct Particle
{
    vec3 position;
    float mass;
    vec4 ellipse_info;
};

layout(std140, binding = 0) buffer ParticleInfo
{
    Particle Info [];
};

mat3 GetRotationMat(float radian)
{
    float s = sin(radian);
    float c = cos(radian);
    return mat3(
        c, -s, 0,
        s, c, 0,
        0, 0, 1);
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    float radian = Info[id].ellipse_info.x * 2 * PI;

    float x = Info[id].ellipse_info.y * cos(radian);
    float y = Info[id].ellipse_info.z * sin(radian);

    mat3 rotation = GetRotationMat(Info[id].ellipse_info.w);
    Info[id].position = rotation * vec3(x, y, 1.f);
    Info[id].ellipse_info.x -= 0.00005f;
} 