#version 440 core
out vec4 FragColor;

float near = 0.1;
float far = 1000;

float LinearizeDepth(float depth)
{
    float z = depth * 2 - 1;
    return (2 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depth), 1);
}