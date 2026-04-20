#pragma once
#ifndef _BASIC_H
#define _BASIC_H
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <SFML/System.hpp>

using namespace std;

//enum RAY_TRACING_ELEMENT
//{
//	TEXTURE_SOLID_COLOR,
//	TEXTURE_CHECKER_TEXTURE,
//	TEXTURE_IMAGE_TEXTURE,
//	MATERIAL_LAMBERTIAN,
//	MATERIAL_METAL,
//	MATERIAL_DIELECTRIC,
//	MATERIAL_DISSUSE_LIGHT,
//	HITTABLE_SPHERE,
//	HITTABLE_TRIANGLE,
//	HITTABLE_QUAD,
//	HITTABLE_MODEL,
//	HITTABLE_LIST,
//};

//using sf::Uint8, sf::Clock;
using glm::normalize, glm::translate;
//using glm::quat;
//using glm::vec2, glm::vec3, glm::vec4;
typedef glm::dvec2 vec2;
typedef glm::dvec3 vec3;
typedef glm::dvec4 vec4;
//typedef glm::dmat3 mat3;
typedef glm::mat3 mat3;
//typedef glm::dmat4 mat4
typedef glm::mat4 mat4;
typedef glm::dquat quat;

inline bool operator < (const vec3& a, const vec3& b)
{
	if (a.x < b.x and a.y < b.y and a.z < b.z)
		return true;
	return false;
}
inline bool operator > (const vec3& a, const vec3& b)
{
	if (a.x > b.x and a.y > b.y and a.z > b.z)
		return true;
	return false;
}
inline bool operator >= (const vec3& a, const vec3& b)
{
	if (a.x >= b.x and a.y >= b.y and a.z >= b.z)
		return true;
	return false;
}
inline bool operator <= (const vec3& a, const vec3& b)
{
	if (a.x <= b.x and a.y <= b.y and a.z <= b.z)
		return true;
	return false;
}
inline vec2 operator * (const vec2& a, const double& b)
{
	return vec2(a.x * b, a.y * b);
}
inline vec2 operator / (const vec2& a, const double& b)
{
	return vec2(a.x / b, a.y / b);
}
inline vec3 operator + (const vec3& a, const vec3& b)
{
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline vec3 operator + (const vec3& b)
{
	return b;
}
inline vec3 operator - (const vec3& a, const vec3& b)
{
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline vec3 operator - (const vec3& b)
{
	return vec3(0, 0, 0) - b;
}
inline vec3 operator * (const vec3& a, const vec3& b)
{
	return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline vec3 operator * (const vec3& v, const double& f)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}
inline vec3 operator * (const double& f, const vec3& v)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}
inline vec3 operator / (const vec3& v, const vec3& f)
{
	return vec3(v.x / f.x, v.y / f.y, v.z / f.z);
}
inline vec3 operator / (const vec3& v, const double& f)
{
	double t = 1.0 / f;
	return vec3(v.x * t, v.y * t, v.z * t);
}
inline vec3 xyz(const vec4& vec)
{
	return vec3(vec.x, vec.y, vec.z);
}
inline void operator += (vec3& a, const vec3& b)
{
	a = a + b;
	return;
}
inline void operator -= (vec3& a, const vec3& b)
{
	a = a - b;
	return;
}

#define PRINT_NETWORK_DEBUG_INFO false

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif


// 求两点距离
inline double distance(const vec2& a, const vec2& b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}
// 求两点距离
inline double distance(const vec3& a, const vec3& b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}
// 求向量模长
inline double get_length(const vec2& a)
{
	return distance(vec2(0, 0), a);
}
// 求向量模长
inline double get_length(const vec3& a)
{
	return distance(vec3(0, 0, 0), a);
}
// 将角度转为弧度
inline double radians(const double& degress)
{
	return degress / 180.f * M_PI;
}
// 将弧度转为角度
inline double degress(const double& radians)
{
	return radians / M_PI * 180.f;
}
// 三维向量点积
inline double dot(const vec3& a, const vec3& b)
{
	double result = 0.f;
	result += a.x * b.x;
	result += a.y * b.y;
	result += a.z * b.z;
	return result;
}
// 三维向量叉乘
inline vec3 cross(const vec3& a, const vec3& b)
{
	return vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
// 二维向量叉乘
inline double cross(const vec2& a, const vec2& b)
{
	return a.x * b.y - a.y * b.x;
}
inline double randomf()
{
	return rand() / (RAND_MAX + 1.0);
}
inline double randomf(const double& min, const double& max)
{
	return min + (max - min) * randomf();
}

// 以角度为单位
inline vec2 toward(float dir, const float& length = 1)
{
	dir = radians(dir);
	return vec2(cos(dir) * length, sin(dir) * length);
}


inline vec3 randomvec3()
{
	return vec3(randomf(), randomf(), randomf());
}
// 在长宽高[min, max]的长方体中生成随机向量
inline vec3 randomvec3(const double& min, const double& max)
{
	return vec3(randomf(min, max), randomf(min, max), randomf(min, max));
}

inline vec3 random_unitvec3()
{
	vec3 p = randomvec3(-1,1);
	while (get_length(p) > 1)
	{
		p = randomvec3(-1, 1);
	}
	return normalize(p);
}
// 根据法线生成在球体表面的单位向量
inline vec3 random_on_hemisphere(const vec3& normal)
{
	vec3 unit_sphere = random_unitvec3();
	//vec3 unit_sphere = normalize(vec3(1, 1, 1));
	if (dot(unit_sphere, normal) > 0.f)
		return unit_sphere;
	return -unit_sphere;
}
// 计算反射光线
inline vec3 reflect(const vec3& normal, const vec3& vec)
{
	return vec - 2.f * double(dot(vec, normal)) * normal;
}
// 计算折射光线
inline vec3 refract(const vec3& uv, const vec3& normal, const double& etai_over_etat)
{
	double cos_theta = fmin(dot(-uv, normal), 1.f);
	vec3 r_out_perp = etai_over_etat * (uv + cos_theta * normal);
	vec3 r_out_parallel = -sqrt(fabs(1.f - pow(get_length(r_out_perp), 2.f))) * normal;
	return r_out_perp + r_out_parallel;
}

inline vec3 rotate_point(const vec3& point, const vec3& pivot, const vec3& axis, double angle_rad) {
	// 1. 平移至原点坐标系
	vec3 translated = point - pivot;

	// 2. 归一化旋转轴
	vec3 norm_axis = normalize(axis);

	// 3. 使用罗德里格斯旋转公式
	double cos_theta = cos(angle_rad);
	double sin_theta = sin(angle_rad);

	// 旋转矩阵计算
	vec3 rotated = translated * cos_theta
		+ cross(norm_axis, translated) * sin_theta
		+ norm_axis * dot(norm_axis, translated) * (1 - cos_theta);

	// 4. 平移回原坐标系
	return rotated + pivot;
}

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

inline void printf(string str, vec2 value)
{
	std::printf(str.c_str(), value.x, value.y);
}
inline void printf(string str, vec3 value)
{
	std::printf(str.c_str(), value.x, value.y, value.z);
}
inline void printf(string str, vec4 value)
{
	std::printf(str.c_str(), value.x, value.y, value.z, value.w);
}
inline void printf(string str, mat3 value)
{
	std::printf(str.c_str(),
		value[0].x, value[0].y, value[0].z,
		value[1].x, value[1].y, value[1].z,
		value[2].x, value[2].y, value[2].z
	);
	return;
}
inline void printf(string str, mat4 value)
{
	std::printf(str.c_str(),
		value[0].x, value[0].y, value[0].z,
		value[1].x, value[1].y, value[1].z,
		value[2].x, value[2].y, value[2].z,
		value[3].x, value[3].y, value[3].z
	);
	return;
}
#endif	// _BASIC_H