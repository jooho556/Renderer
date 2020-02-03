#version 440 core

in vec3 o_texcoord;
in vec3 ray_dir;
in vec3 o_pos;

out vec4 frag_color;

uniform highp sampler3D noise;
uniform highp sampler2D transfer_func;
uniform vec3 eye_pos;
uniform float time;
uniform mat4 view;
uniform vec2 iResolution = vec2(1024, 1024);

 uniform float focal_length = 0.785;

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
    vec3 uvw;
    uvw.xy= (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
    uvw.z = -focal_length;

    Ray ray = Ray(vec3(0), normalize(uvw));
    Cube cube = Cube((view * vec4(0, 0, 0, 1)).xyz, (view * vec4(1, 1, 1, 1)).xyz);

    vec3 col = vec3(1.);

    float t0, t1;
    if(RayBoxIntersection(ray, cube, t0, t1))
        col = vec3((t1 - t0) / sqrt(3));
    else
        col = vec3(0.4f, 0.8, 0.4);

    frag_color = vec4(col, 1.);
}

