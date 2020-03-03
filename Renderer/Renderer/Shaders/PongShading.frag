#version 440 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct DirLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//#define NR_POINT_LIGHTS 4
//uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight{
    float inner_cutoff;
    float outer_cutoff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
     //Properties
     vec3 norm = normalize(Normal);
     vec3 viewDir = normalize(-FragPos);

     //Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

     //Point lights
     //for(int i = 0; i < NR_POINT_LIGHTS; ++i)
     //    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);

      //Spot light
      //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

      FragColor = vec4(result, 1.0);
}

#define SHININESS 32

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    //Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS);

    vec3 diffuse_map = vec3(texture(texture_diffuse1, TexCoords));
    vec3 ambient = light.ambient * diffuse_map; //use diffuse map
    vec3 diffuse = light.diffuse * diff * diffuse_map;
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    //Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS);
    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    //Combine
    vec3 diffuse_map = vec3(texture(texture_diffuse1, TexCoords));
    vec3 ambient = light.ambient * diffuse_map;
    vec3 diffuse = light.diffuse * diff * diffuse_map;
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //Light Direction == View Direction
    float theta = viewDir.z; // = dot(vec3(0, 0, 1), lightDir);
    float epsilon = light.inner_cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.f, 1.f);

    //Diffuse shading
    float diff = max(dot(normal, viewDir), 0.0);
    //Specular shading
    vec3 reflectDir = reflect(-viewDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), SHININESS);
    //attenuation
    float distance = length(-fragPos);
    float attenuation = 1.f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    //Combine
    vec3 diffuse_map = vec3(texture(texture_diffuse1, TexCoords));
    vec3 ambient = light.ambient * diffuse_map;
    vec3 diffuse = light.diffuse * diff * diffuse_map;
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}