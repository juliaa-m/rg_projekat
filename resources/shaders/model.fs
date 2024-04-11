#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

};

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

#define N_POINT_LIGHTS 3

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform DirLight dirlight;
uniform PointLight pointlight[N_POINT_LIGHTS];
uniform Material material;
uniform SpotLight spotlight;

uniform vec3 viewPos;
uniform bool blinn;
// calculates the color when using a point light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    //ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    //diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0f;

    //Blinn-Phong
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir),0.0), material.shininess);
    }else{
        spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    }

    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    //ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    //diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0f;

    //Blinn-Phong
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir),0.0), material.shininess);
    }else{
        spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    }
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;


    //attenuation
    float d = length(light.position - fragPos);
    float att = 1.0/(d*d);
    ambient *= att;
    diffuse *= att;
    specular *= att;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    //ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    //diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal),0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0f;

    //Blinn-Phong
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir),0.0), material.shininess);
    }else{
        spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    }

    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;


    //attenuation
    float d = length(light.position - fragPos);
    float att = 1.0/(d*d);
    ambient *= att;
    diffuse *= att;
    specular *= att;
    //spotlight
    float theta = dot(-lightDir, normalize(light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = CalcDirLight(dirlight, normal, viewDir);

    for(int i = 0; i < N_POINT_LIGHTS; i++){
        result += CalcPointLight(pointlight[i], normal, FragPos, viewDir);
    }

    result += CalcSpotLight(spotlight, normal, FragPos, viewDir);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
        if(brightness > 1.0)
            BrightColor = vec4(result, 1.0);
        else
            BrightColor = vec4(0.0, 0.0, 0.0, 1.0);


    FragColor = vec4(result, 1.0);
}