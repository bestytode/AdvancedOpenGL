#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    vec3 originalColor = texture(texture1, TexCoords).rgb;
    FragColor = vec4((vec3(1.0f) - originalColor), 1.0f);
}
