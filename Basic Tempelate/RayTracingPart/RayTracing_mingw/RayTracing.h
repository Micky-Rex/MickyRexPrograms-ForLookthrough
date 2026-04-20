/*
* 最低 ISO-C++14 标准
* 基于sfml2.6.1的基本3D光线追踪引擎
* 保证 %INCLUDE% 路径包含sfml头文件路径
* 保证 %LIB%\SFML 路径包含sfml静态链接库文件路径
*
* 依赖sfml-window.lib, sfml-system.lib, opengl32.lib, freetype.lib, gdi32.lib, sfml-graphics.lib
*/
#pragma once
#include "interval.h"
#include "Graphics.h"
#include "camera.h"
#include "hittable.h"

namespace BasicRayTracing2D {
	//Color ray_color(ray& r, const hittable& world)
	//{
	//	hit_record rec;
	//	if (world.hit(r, interval(0, infinity), rec))
	//	{
	//		return vec_to_color(rec.normal * 0.5f + vec3(0.5f, 0.5f, 0.5f));
	//	}
	//	
	// a = 0.5f * (r.normal_direction.y + 1.f);
	//	vec3 color = ((1.f - a) * vec3(1.f, 1.f, 1.f) + a * vec3(0.5f, 0.7f, 1.f));
	//	return vec_to_color(color);
	//}

	//// 判断是否与球体相交
	//double hit_sphere(const vec3& center, double radius, const ray& r)
	//{
	//	vec3 oc = center - r.original;
	//	double a = dot(r.direction, r.direction);
	//	double h = dot(r.direction, oc);
	//	double c = dot(oc, oc) - radius * radius;
	//	double discriminant = h * h - a * c;
	//	if (discriminant < 0)
	//	{
	//		return -1.0;
	//	}
	//	else
	//	{
	//		return (h - sqrt(discriminant)) / a;
	//	}
	//}

	// 获取光线击中的颜色
	//Color ray_color(ray& r)
	//{
	//	double t = hit_sphere(vec3(0, 0, -1), 0.5, r);
	//	if (t > 0.0)
	//	{
	//		vec3 N = normalize(r.at(t) - vec3(0, 0, -1));
	//		return vec_to_color(N * 0.5f + vec3(0.5f, 0.5f, 0.5f));
	//	}
	//	double a = 0.5f * (r.normal_direction.y + 1.f);
	//	vec3 color = ((1.f - a) * vec3(1.f, 1.f, 1.f) + a * vec3(0.5f, 0.7f, 1.f));
	//	return vec_to_color(color);
	//}
}