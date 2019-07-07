#version 400 core

struct Material{
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
     float shininess;
};

struct Light{
     vec3 position;
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;
};

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;
uniform vec3 lightPos;

void main()
{
    //Ambient
    vec3 ambient = light.ambient * material.ambient;

    //Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    //Specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0f);
}

