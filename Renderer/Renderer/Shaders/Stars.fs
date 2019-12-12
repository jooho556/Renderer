#version 440 core

out vec4 out_color;

uniform sampler2D tex;

void main()
{
    out_color = texture(tex, gl_PointCoord);
}