#version 440 core

in vec2 tex_coord;

uniform sampler2D stars;
uniform sampler2D noise_source;

uniform vec3 color = vec3(0.4f, 0.2f, 0.4f);
uniform vec2 offset = vec2(0.f, 0.f);
uniform float scale = 1.f;
uniform float density = 0.25f;
uniform float falloff = 8.f;

uniform float screen_width;
uniform float screen_height;

#define M_PI 3.14159265358979323846

float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}
float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}
float rand (vec2 co, float l, float t) {return rand(vec2(rand(co, l), t));}

float perlin(vec2 p, float dim, float time) {
	vec2 pos = floor(p * dim);
	vec2 posx = pos + vec2(1.0, 0.0);
	vec2 posy = pos + vec2(0.0, 1.0);
	vec2 posxy = pos + vec2(1.0);
	
	float c = rand(pos, dim, time);
	float cx = rand(posx, dim, time);
	float cy = rand(posy, dim, time);
	float cxy = rand(posxy, dim, time);
	
	vec2 d = fract(p * dim);
	d = -0.5 * cos(d * M_PI) + 0.5;
	
	float ccx = mix(c, cx, d.x);
	float cycxy = mix(cy, cxy, d.x);
	float center = mix(ccx, cycxy, d.y);
	
	return center * 2.0 - 1.0;
}

// p must be normalized!
float perlin(vec2 p, float dim) {
	return perlin(p, dim, .01);
}

   float normalnoise(vec2 p) {
    return perlin(p, 0.1f) * 0.5 + 0.5;
}
float noise(vec2 p) {
    p += offset;
    const int steps = 5;
    float scale = pow(2.0, float(steps));
    float displace = 0.0;
    for (int i = 0; i < steps; i++) {
        displace = normalnoise(p * scale + displace);
        scale *= 0.5;
    }
    return normalnoise(p + displace);
}

void main()
{
    vec3 s = texture2D(stars, tex_coord).rgb;
    //vec3 s = vec3(0.f);
    float n = noise(gl_FragCoord.xy * scale * 1.0);
    n = pow(n + density, falloff);
    gl_FragColor = vec4(mix(s, color, n), 1.f);
}