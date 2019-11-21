#version 440 core

uniform float PI;
uniform float dt = 0.0005f;

layout(local_size_x = 1000) in;

layout(std430, binding = 0) buffer Pos
{
    vec4 Position [];
};

layout(std430, binding = 1) buffer M
{
    float Mass [];
};

layout(std430, binding = 2) buffer EInfo
{
    vec4 Info [];
};

layout(std430, binding = 3) buffer ErInfo
{
    mat4 rInfo [];
};

void main()
{
    uint id = gl_GlobalInvocationID.x;
    float radian = Info[id].x * 2 * PI;

    float x = Info[id].y * cos(radian);
    float y = Info[id].z * sin(radian);

    Position[id] = rInfo[id] * vec4(x, y, 0.f, 1.f);
    //float x_vel = -2 * PI * Info[id].y * sin(radian);
    //float y_vel = 2 * PI * Info[id].z * cos(radian);

    //Velocity[id] = vec4(x_vel, y_vel, 0.f, 1.f);
    //Position[id] = vec4(Position[id].xyz + Velocity[id].xyz * dt, 1.f);
    Info[id].x -= 0.00005f;
} 