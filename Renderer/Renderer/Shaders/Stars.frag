#version 440 core

out vec4 FragColor;

in vec3 out_color;

uniform sampler2D tex;
uniform float alpha = 10.f;

void main()
{
    FragColor = vec4(out_color.xyz, texture(tex, gl_PointCoord).a / alpha);
}