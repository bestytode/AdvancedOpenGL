#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    vec4 originalColor = vec4(texture(texture1, TexCoords).rgb, 1.0f);
    FragColor = vec4(1.0f) - originalColor;
}
