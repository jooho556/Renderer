#version 440 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model_to_view;
uniform mat4 projection;
uniform mat4 normalMat;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

out vec4 finalColor;

void main()
{
     vec4 pos_viewcoord =  model_to_view * vec4(aPos, 1.f);
     gl_Position = projection * pos_viewcoord;
     vec3 Normal = mat3(normalMat) * aNormal;
     vec3 FragPos = vec3(pos_viewcoord);

    //Ambient
    float ambient_factor = 0.1f;
    vec3 ambient = ambient_factor * lightColor;

    //Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //Specular
    float specular_factor = 1.f;
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specular_factor * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    finalColor = vec4(result, 1.0f);
}
