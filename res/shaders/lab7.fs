#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

uniform vec3 lightPos;         // Position of the light source for positional light
uniform vec3 lightColor;       // Color of the light source
uniform vec3 lightDirection;   // Direction of the light source for directional light
uniform vec3 viewPos;          // Camera (viewer) position in world space

void PositionalLight()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    float distanceToLight = length(lightPos - FragPos);
  
    // Ambient component
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular component
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128.0);
    vec3 specular = specularStrength * spec * lightColor;

    // Attenuation (for positional light only)
    float constant = 1.0;   // Constant attenuation factor
    float linear = 0.09;    // Linear attenuation factor
    float quadratic = 0.032; // Quadratic attenuation factor
    float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));
    
    // Combine all components
    vec3 result = 4.0f * (ambient + diffuse + specular) * Color * attenuation;
    FragColor = vec4(result, 1.0);
}

void DirectionalLight()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-lightDirection); // Assuming lightDirection points from light to the object

    // Ambient component
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular component
    float specularStrength = 0.5;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 128.0);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all components
    vec3 result = (ambient + diffuse + specular) * Color;
    FragColor = vec4(result, 1.0);
}

void main()
{
    //DirectionalLight();
    PositionalLight();
}