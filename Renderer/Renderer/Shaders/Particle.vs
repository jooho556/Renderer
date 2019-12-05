#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aMass;
layout(location = 2) in vec4 aEllipseInfo;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;
uniform float tick = 0.05f;
uniform float point_size_scale = 1.f;

out vec3 out_color;

void main()
{
    out_color = color;

    gl_Position = projection * view * model * vec4(aPos.xy, 0, 1);
    gl_PointSize = aMass  * point_size_scale * (sin(tick + gl_VertexID) + 1);

    float r_sqr = aPos.x * aPos.x + aPos.y * aPos.y;
    if(aMass > 29 && r_sqr > 100)
        out_color = vec3(.9f, 0.5f, 0.5f);
}