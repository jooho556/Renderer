#version 400 core

struct Material{
     sampler2D diffuse;
     sampler2D specular;
     float shininess;
};

struct Light{
     //vec3 position;
     vec3 ambient;
     vec3 diffuse;
     vec3 specular;

     float constant;
     float linear;
     float quadratic;

     float inner_cutoff;
     float outer_cutoff;
};

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;
uniform Light light;

void main()
{
     vec3 lightDir = normalize(-FragPos);
     float epsilon = light.inner_cutoff - light.outer_cutoff;
     float theta = lightDir.z;
     float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.f, 1.f);

      //Ambient
     vec3 texture_diffuse = vec3(texture(material.diffuse, TexCoords));
     vec3 ambient = light.ambient * texture_diffuse;

     //Diffuse
     vec3 normal = normalize(Normal);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 diffuse = light.diffuse * diff * texture_diffuse;

     //Specular
     vec3 viewDir = normalize(-FragPos);
     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
     vec3 specular = light.specular * vec3(texture(material.specular, TexCoords)) * spec;

     float distance = length(-FragPos);
     float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

     ambient *= attenuation;
     diffuse *= attenuation;
     specular *= attenuation;

     diffuse *= intensity;
     specular *= intensity;

     FragColor = vec4(ambient + diffuse + specular, 1.0f);
}

