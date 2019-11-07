#version 440 core
#define EPS2 0.2

layout(local_size_x = 1000) in;

uniform uint particle_num = 10000;
uniform float dt = 0.00005f;
uniform float damping = 0.9999;
uniform float G = 2.f;

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

vec3 GetForce(in vec4 p1, in vec4 p2)
{
    vec3 radius = p2.xyz - p1.xyz;
    float r_sqr = dot(radius, radius) + EPS2;
    float r_sixth = r_sqr * r_sqr * r_sqr;
    float inv_dist_cube = inversesqrt(r_sixth);
    return radius * inv_dist_cube;
}

void main()
{
    uint id = gl_GlobalInvocationID.x;
    vec4 pos = Position[id];
    vec3 net_force = vec3(0, 0, 0);

    for (uint i = 0; i < particle_num; ++i)
    {
        vec4 particle = Position[i];

        vec3 force = GetForce(pos, particle);

        if (i != id)
            net_force += force;
    }

    vec3 final_vel = dt * net_force * G + Velocity[id].xyz * damping;
    Velocity[id] = vec4(final_vel, 0.f);
    Position[id] = vec4(pos.xyz + dt * Velocity[id].xyz, 1.f);

    //vec3 net_force = vec3(0, 0, 0);

    //for(uint i = 0; i < particle_num; ++i)
    //{
    //    float dist = distance(Position[i], Position[id]);
    //    float g = 1.0 / dist * 0.1;

    //    vec3 dir = Position[i].xyz - Position[id].xyz;
    //    dir = dir / (length(dir) + 1e-10);

    //    vec3 force = g * dir * 0.001;

    //    if (i != id)
    //        net_force += force;
    //}

    //Position[id] += Velocity[id] + vec4(net_force / 2.f * 0.005f, 0.f);
    //Velocity[id] += vec4(net_force * 0.005f, 0.f);
} 