#version 440 core

out vec4 frag_color;

uniform float screen_width;
uniform float screen_height;

float rand(vec2 st)
{
    return fract(sin(dot(st.xy,
        vec2(12.9898, 78.233))) * 43758.5453123);
}

void main()
{
    vec2 st = gl_FragCoord.xy / vec2(screen_width, screen_height);
    
    float n = rand(st);

    gl_FragColor = vec4(vec3(n), 1);

}