#version 440 core

in vec3 o_texcoord;
in vec3 o_pos;

out vec4 frag_color;

uniform highp sampler3D noise;
uniform highp sampler2D transfer_func;
uniform vec3 eye_pos;
uniform float time;

const float max_distance = sqrt(3);
const int num_sample = 512;
const float step_size = max_distance / float(num_sample);

 struct Cube
 {
    vec3 min;
    vec3 max;
 };

 struct Ray
 {
    vec3 origin;
    vec3 dir;
 };

 //Ray-Cube intersection test - Intersection interval : [t0, t1]
 bool RayBoxIntersection(Ray r, Cube cube, out float t0, out float t1)
 {
     //IEEE float point correction (-0 == 0)
     vec3 inverse_dir = 1. / r.dir;

     //Intersection points (t values) where ray and xyz axes of cubemin intersect
     vec3 tmin_intersection = inverse_dir * (cube.min - r.origin);
     //t values where ray and xyz axes of cubemax intersect
     vec3 tmax_intersection = inverse_dir * (cube.max - r.origin);

     //we have 6 t values by now
     //gets 3 t values which is less than other t values
     vec3 tmin = min(tmin_intersection, tmax_intersection);
     //gets the greatest one
     vec2 tmin_compare = max(tmin.xx, tmin.yz);
     t0 = max(tmin_compare.x, tmin_compare.y);

     //gets 3 t values which is  greater than other t values
     vec3 tmax = max(tmin_intersection, tmax_intersection);
     //gets the smallest one
     vec2 tmax_compare = min(tmax.xx, tmax.yz);
     t1 = min(tmax_compare.x, tmax_compare.y);

     return t0 <= t1;
 }

void main()
{ 
    vec3 ray_origin = eye_pos;
    vec3 ray_dir = normalize(o_pos - eye_pos);
    Ray ray = Ray(eye_pos, ray_dir);
    Cube cube = Cube(vec3(0, 0, 0), vec3(1, 1, 1));
    vec4 col = vec4(0.);

    float t0, t1;
    bool is_intersecting = RayBoxIntersection(ray, cube, t0, t1);
    if(t0 < 0.) t0 = 0.; // if t0 is behind the camera, ray starts from camera (t = 0)

    if(is_intersecting)
    {
        vec3 ray_start = ray_origin + ray_dir * t0;
        vec3 ray_end = ray_origin + ray_dir * t1;

        vec3 pos = ray_start;
        vec3 step = normalize(ray_end - ray_start) * step_size;
        float travel = distance(ray_end, ray_start);
        for(int i = 0; i < num_sample && travel > 0.; pos += step, travel -= step_size)
        {
            float val = texture(noise, pos).r;
            
            if(col.a > 0.95) break;
            col += val  / 100.;
        }
    }
    else
    {
        col = vec4(1.);
    }
    
    frag_color = col;
}

