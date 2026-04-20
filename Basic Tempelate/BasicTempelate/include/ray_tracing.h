/*
* 最低 ISO-C++14 标准
* 基于sfml2.6.1的基本3D光线追踪引擎
* 保证 %INCLUDE% 路径包含sfml头文件路径
* 保证 %LIB%\SFML 路径包含sfml静态链接库文件路径
*
* 依赖sfml-window.lib, sfml-system.lib, opengl32.lib, freetype.lib, gdi32.lib, sfml-graphics.lib
*/
#pragma once

typedef int RAY_TRACING_ELEMENT;
const int TEXTURE_SOLID_COLOR = 0;
const int TEXTURE_CHECKER_TEXTURE = 1;
const int TEXTURE_IMAGE_TEXTURE = 2;
const int MATERIAL_LAMBERTIAN = 3;
const int MATERIAL_METAL = 4;
const int MATERIAL_DIELECTRIC = 5;
const int MATERIAL_DISSUSE_LIGHT = 6;
const int HITTABLE_SPHERE = 7;
const int HITTABLE_TRIANGLE = 8;
const int HITTABLE_QUAD = 9;
const int HITTABLE_MODEL = 10;
const int HITTABLE_LIST = 11;

#include "ray_tracing/camera.h"
