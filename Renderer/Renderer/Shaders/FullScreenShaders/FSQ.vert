#version 440 core
layout(location = 0) in vec3 aPos;
uniform mat4 model_to_view;
void main()
{
     gl_Position = ndc_to_device * vec4(aPos, 1.f);     
}
