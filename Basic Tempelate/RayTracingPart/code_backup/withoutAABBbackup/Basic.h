#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <memory>
#include <limits>
#include <SFML/System.hpp>

using namespace sf;
using namespace std;

typedef Vector2<double> vec2;
typedef Vector3<double> vec3;
typedef Vector3i vec3i;
/*
struct vec2
{
	double x, y;
	vec2() = default;
	vec2(double x, double y) :x(x), y(y) {};
};
vec2 operator + (const vec2& a, const vec2& b)
{
	return vec2(a.x + b.x, a.y + b.y);
}
vec2 operator + (const vec2& b)
{
	return b;
}
vec2 operator - (const vec2& a, const vec2& b)
{
	return vec2(a.x - b.x, a.y - b.y);
}
vec2 operator - (const vec2& b)
{
	return vec2(0, 0) - b;
}
vec2 operator * (const vec2& v, const double& f)
{
	return vec2(v.x * f, v.y * f);
}
vec2 operator * (const double& f, const vec2& v)
{
	return vec2(v.x * f, v.y * f);
}
vec2 operator * (const vec2& a, const vec2& b) {
	return vec2(a.x * b.x, a.y * b.y);
}
vec2 operator / (const vec2& v, const vec2& f)
{
	return vec2(v.x / f.x, v.y / f.y);
}
vec2 operator / (const vec2& v, const double& f)
{
	return vec2(v.x / f, v.y / f);
}
vec2 operator / (const double& f, const vec2& v)
{
	return vec2(f / v.x, f / v.y);
}
void operator += (vec2& a, const vec2& b)
{
	a = a + b;
	return;
}
void operator -= (vec2& a, const vec2& b)
{
	a = a - b;
	return;
}
void operator *= (vec2& a, const vec2& b)
{
	a = a * b;
	return;
}
void operator *= (vec2& a, const double& b)
{
	a = a * b;
	return;
}
*/


vec2 operator * (const vec2& a, const double& b)
{
	return vec2(a.x * b, a.y * b);
}
vec2 operator / (const vec2& a, const double& b)
{
	return vec2(a.x / b, a.y / b);
}

vec3 operator + (const vec3& a, const vec3& b)
{
	return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
vec3 operator + (const vec3& b)
{
	return b;
}
vec3 operator - (const vec3& a, const vec3& b)
{
	return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
vec3 operator - (const vec3& b)
{
	return vec3(0, 0, 0) - b;
}

vec3 operator * (const vec3& a, const vec3& b)
{
	return vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}
vec3 operator * (const vec3& v, const double& f)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}
vec3 operator * (const double& f, const vec3& v)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}
vec3 operator / (const vec3& v, const vec3& f)
{
	return vec3(v.x / f.x, v.y / f.y, v.z / f.z);
}
vec3 operator / (const vec3& v, const double& f)
{
	return vec3(v.x / f, v.y / f, v.z / f);
}
//vec3 operator / (const double& f, const vec3& v)
//{
//	return vec3(f / v.x, f / v.y, f / v.z);
//}

void operator += (vec3& a, const vec3& b)
{
	a = a + b;
	return;
}
void operator -= (vec3& a, const vec3& b)
{
	a = a - b;
	return;
}
//void operator *= (vec3& a, const vec3& b)
//{
//	a = a * b;
//	return;
//}
//void operator *= (vec3& a, const double& b)
//{
//	a = a * b;
//	return;
//}

//#define COMPILE_WITHOUT_GRAPHICS
#define PRINT_NETWORK_DEBUG_INFO false

#ifdef COMPILE_WITHOUT_GRAPHICS

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

struct Color
{
	static const Color Black;       //!< Black predefined color
	static const Color White;       //!< White predefined color
	static const Color Red;         //!< Red predefined color
	static const Color Green;       //!< Green predefined color
	static const Color Blue;        //!< Blue predefined color
	static const Color Yellow;      //!< Yellow predefined color
	static const Color Transparent; //!< Transparent (black) predefined color

	Uint8 r, g, b, a;

	Color()
	{
		*this = Color(0, 0, 0);
		return;
	}
	Color(Uint8 _red, Uint8 _green, Uint8 _blue, Uint8 _alpha = 255U)
	{
		this->r = _red;
		this->g = _green;
		this->b = _blue;
		this->a = _alpha;
	}
};
struct Image
{
private:
	unsigned int width;
	unsigned int height;
	Color* image;
public:
	Image() = default;
	Image(unsigned int _width, unsigned int _height)
	{
		this->width = _width;
		this->height = _height;
		this->image = new Color[width * height]{};
		return;
	}
	void create(unsigned int _width, unsigned int _height)
	{
		*this = Image(_width, _height);
		return;
	}
	void setPixel(unsigned int x, unsigned int y, Color color)
	{
		if (x > width or y > height)
		{
			printf("[Error]: Array out of bounds!\n");
			return;
		}
		image[x + y * width] = color;
		return;
	}
	void* getData(unsigned int& _width, unsigned int& _height)
	{
		_width = this->width;
		_height = this->height;
		return this->image;
	}
	void saveToFile(string filename)
	{
		stbi_write_png("output.png", width, height, 4, getData(width, height), width*4);
	}
};
const Color Color::Black(0U, 0U, 0U);
const Color Color::White(255U, 255U, 255U);
const Color Color::Red(255U, 0U, 0U);
const Color Color::Green(0U, 255U, 0U);
const Color Color::Blue(0U, 0U, 255U);
const Color Color::Transparent(0U, 0U, 0U, 255U);

#endif

#ifndef pi
#define pi 3.1415926535
#endif


inline Vector3f toVector3f(const vec3& vec)
{
	return Vector3f(vec.x, vec.y, vec.z);
}
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
	return degress / 180.f * pi;
}
// 将弧度转为角度
inline double degress(const double& radians)
{
	return radians / pi * 180.f;
}
// 归一化向量
inline vec2 normalize(const vec2& vec)
{
	float model = get_length(vec) + 1e-150;
	return vec / model;
}
// 归一化向量
inline vec3 normalize(const vec3& vec)
{
	double model = get_length(vec) + 1e-150;
	return vec / model;
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
// 三维向量叉乘
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

// 四元数类
class quaternion {
public:
	double w, x, y, z;

	quaternion() : w(1), x(0), y(0), z(0) {}
	quaternion(double w, double x, double y, double z)
		: w(w), x(x), y(y), z(z) {
	}

	// 从欧拉角创建四元数（弧度制）
	static quaternion from_euler(double roll, double pitch, double yaw) {
		double cy = cos(yaw * 0.5);
		double sy = sin(yaw * 0.5);
		double cp = cos(pitch * 0.5);
		double sp = sin(pitch * 0.5);
		double cr = cos(roll * 0.5);
		double sr = sin(roll * 0.5);

		return quaternion(
			cr * cp * cy + sr * sp * sy,
			sr * cp * cy - cr * sp * sy,
			cr * sp * cy + sr * cp * sy,
			cr * cp * sy - sr * sp * cy
		);
	}

	// 四元数乘法
	quaternion operator*(const quaternion& q) const {
		return quaternion(
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		);
	}
	vec3 to_euler() const {
		// roll (x-axis rotation)
		double sinr_cosp = 2 * (w * x + y * z);
		double cosr_cosp = 1 - 2 * (x * x + y * y);
		double roll = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2 * (w * y - z * x);
		double pitch;
		if (fabs(sinp) >= 1) {
			// 处理90度的情况
			pitch = copysign(pi / 2, sinp);
		}
		else {
			pitch = asin(sinp);
		}

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		double yaw = atan2(siny_cosp, cosy_cosp);

		return vec3(roll, pitch, yaw);
	}

	// 归一化
	quaternion normalized() const {
		double len = sqrt(w * w + x * x + y * y + z * z);
		return (len > 0) ? quaternion(w / len, x / len, y / len, z / len) : *this;
	}

	// 转换为旋转矩阵
	void to_rotation_matrix(vec3 matrix[3]) const {
		double n = w * w + x * x + y * y + z * z;
		double s = (n > 0) ? 2 / n : 0;

		double wx = s * w * x, wy = s * w * y, wz = s * w * z;
		double xx = s * x * x, xy = s * x * y, xz = s * x * z;
		double yy = s * y * y, yz = s * y * z, zz = s * z * z;

		matrix[0] = vec3(1 - (yy + zz), xy - wz, xz + wy);
		matrix[1] = vec3(xy + wz, 1 - (xx + zz), yz - wx);
		matrix[2] = vec3(xz - wy, yz + wx, 1 - (xx + yy));
	}
};