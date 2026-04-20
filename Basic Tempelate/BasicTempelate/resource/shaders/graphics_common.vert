#version 460 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aColor;

out vec3 aFragNormal;
out vec2 aFragTexCoord;
out vec4 aFragColor;

void main() {
    gl_Position = vec4(aPosition, 1.0);
    aFragNormal = aNormal;
    aFragTexCoord = aTexCoord;
    aFragColor = aColor;
}