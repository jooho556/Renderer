#version 440 core

layout(local_size_x = 1000) in;

layout(std430, binding = 0) buffer Pos
{
    vec4 Position [];
};

layout(std430, binding = 1) buffer Vel
{
    vec4 Velocity [];
};

layout(std430, binding = 2) buffer M
{
    float Mass [];
};


void main()
{
    uint id = gl_GlobalInvocationID.x;

    
} 