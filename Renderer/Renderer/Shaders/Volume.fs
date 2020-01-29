#version 440 core

in vec3 o_texcoord;
in vec3 ray_dir;

out vec4 frag_color;

uniform highp sampler3D noise;
uniform highp sampler2D transfer_func;
uniform vec3 eye_pos;
uniform float time;

vec2 intersect_box(vec3 orig, vec3 dir) {
    const vec3 box_min = vec3(0);
    const vec3 box_max = vec3(1);
    vec3 inv_dir = 1.0 / dir;
    vec3 tmin_tmp = (box_min - orig) * inv_dir;
    vec3 tmax_tmp = (box_max - orig) * inv_dir;
    vec3 tmin = min(tmin_tmp, tmax_tmp);
    vec3 tmax = max(tmin_tmp, tmax_tmp);
    float t0 = max(tmin.x, max(tmin.y, tmin.z));
    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(t0, t1);
}

void main()
{
    vec3 nray_dir = normalize(ray_dir);

    vec2 ray_interval = intersect_box(eye_pos, nray_dir);
    if(ray_interval.x > ray_interval.y) discard;

    ray_interval.x = max(ray_interval.x, 0.f);
    vec3 dt_vec = 1.f / abs(nray_dir);
    float dt = min(dt_vec.x, min(dt_vec.y, dt_vec.z));

    vec4 color = vec4(0.0);
    vec3 p = eye_pos + ray_interval.x * nray_dir;
    for(float t = ray_interval.x; t < ray_interval.y; t += dt)
    {
         float val = texture(noise, p).r;
         vec4 val_color = vec4(val);//vec4(texture(transfer_func, vec2(val, 0.5)).rgb, val);

         color.rgb += (1.0 - color.a) * val_color.a * val_color.rgb;
         color.a += (1.0 - color.a) * val_color.a;

         if(color.a >= 0.95) break;
         p += nray_dir * dt;
    }
    frag_color = color;
}