/*
* 最低 ISO-C++14 标准
* 基于sfml2.6.1的基本2D单窗口渲染接口
* 保证 %INCLUDE% 路径包含sfml头文件路径
* 保证 %LIB%\SFML 路径包含sfml静态链接库文件路径
* 
* 依赖sfml-window.lib, sfml-system.lib, opengl32.lib, freetype.lib, gdi32.lib, sfml-graphics.lib
*/
#pragma once
#include "Basic.h"
#ifndef COMPILE_WITHOUT_GRAPHICS
#include <SFML/Graphics.hpp>
#endif

const int window_width = 1920;
const double window_aspect_ratio = 16.f / 9.f;
const int window_height = int(window_width / window_aspect_ratio);

inline double linear_to_gamma(double linear_component)
{
	if (linear_component > 0) return sqrt(linear_component);
	return 0;
}
inline vec3 linear_to_gamma(vec3 linear_component)
{
	return vec3(
		linear_to_gamma(linear_component.x),
		linear_to_gamma(linear_component.y),
		linear_to_gamma(linear_component.z)
	);
}