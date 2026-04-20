#version 460 core

in vec4 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform bool useTexture;

void main()
{
    if (useTexture) {

    }
    else {
    	FragColor = Color;
    }
    FragColor = vec4(1,0,0,1);
}