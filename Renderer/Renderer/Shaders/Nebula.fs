#version 440 core

out vec4 FragColor;

in vec3 out_color;
uniform sampler2D tex;

void main()
{
    FragColor = vec4(out_color.xyz, texture(tex, gl_PointCoord).a / 10.f);
}