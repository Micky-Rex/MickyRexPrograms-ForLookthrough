#version 460 core
precision highp float;

// Ray tracing element constants
const int TEXTURE_SOLID_COLOR      = 0;
const int TEXTURE_CHECKER_TEXTURE  = 1;
const int TEXTURE_IMAGE_TEXTURE    = 2;
const int MATERIAL_LAMBERTIAN      = 3;
const int MATERIAL_METAL           = 4;
const int MATERIAL_DIELECTRIC      = 5;
const int MATERIAL_DISSUSE_LIGHT   = 6;
const int HITTABLE_SPHERE         = 7;
const int HITTABLE_TRIANGLE       = 8;
const int HITTABLE_QUAD           = 9;
const int HITTABLE_MODEL          = 10;
const int HITTABLE_LIST           = 11;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPosition.xyz, 1.0);
    TexCoord = aTexCoord;
}