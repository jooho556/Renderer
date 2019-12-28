#version 440 core

in vec2 tex_coord;
out vec4 out_color;

uniform sampler2D stars;
uniform sampler2D noise;
uniform vec3 color = vec3(0.6f, 0.1f, 0.4f);

void main()
{
    vec3 s = texture2D(stars, tex_coord).rgb;
    vec3 n = texture2D(noise, tex_coord).rgb;
	   out_color = vec4(mix(s, color, n), 1.f);
}