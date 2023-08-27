#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{    
    float ior = 1.0f/ 1.52f;
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = refract(I, normalize(Normal), ior);
    vec3 color = texture(skybox, R).rgb;
    FragColor = vec4(color, 1.0f);
}