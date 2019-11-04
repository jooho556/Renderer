#version 440 core

layout(local_size_x = 1000) in;

uniform float Gravity1 = 100;
uniform vec3 BlackHolePos1;

uniform float Gravity2 = 100;
uniform vec3 BlackHolePos2;

uniform float ParticleMass = 1;
uniform float ParticleInvMass = 1.0;

uniform float dt = 0.0005;
uniform float MaxDist = 10;

layout(std430, binding = 0) buffer Pos
{
    vec4 Position [];
};

layout(std430, binding = 1) buffer Vel
{
    vec4 Velocity [];
};

void main()
{
    uint id = gl_GlobalInvocationID.x;

    vec3 pos = Position[id].xyz;

    //Force from bh1
    vec3 d = BlackHolePos1 - pos;
    float dist = length(d);
    vec3 force = (Gravity1 / dist) * normalize(d);

    //Force from bh2
    d = BlackHolePos2 - pos;
    dist = length(d);
    force += (Gravity2 / dist) * normalize(d);

    if(dist > MaxDist)
    {
        Position[id] = vec4(0, 0, 0, 1);
    }
    else
    {
        vec3 accel = force * ParticleInvMass;
        Position[id] = vec4(pos + Velocity[id].xyz * dt + 0.5 * accel * dt * dt, 1.f);
        Velocity[id] = vec4(Velocity[id].xyz + accel * dt, 0);
    }

}