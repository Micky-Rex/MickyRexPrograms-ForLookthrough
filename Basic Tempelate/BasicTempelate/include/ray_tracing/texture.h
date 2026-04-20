#pragma once
#include "../Basic.h"
#include "image.h"

class texture
{
public:
	virtual ~texture() = default;
	virtual RAY_TRACING_ELEMENT type() const = 0;
	virtual vec3 value(double& u, double& v, const vec3& p) const = 0;
};

class solid_color : public texture
{
public:
	solid_color(const vec3& albedo) : albedo(albedo) {}
	solid_color(const double& r, const double& g, const double& b) : solid_color(vec3(r, g, b)) {}
	inline RAY_TRACING_ELEMENT type() const override { return TEXTURE_SOLID_COLOR; }
	vec3 value(double& u, double& v, const vec3& p) const override { return albedo; }

//private:
	vec3 albedo;
};

class checker_texture : public texture
{
public:
	checker_texture(const double& scale, shared_ptr<texture> even, shared_ptr<texture> odd) : inv_scale(1.0 / scale), even(even), odd(odd) {}
	checker_texture(const double& scale, const vec3& c1, const vec3& c2) : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}
	inline RAY_TRACING_ELEMENT type() const override { return TEXTURE_CHECKER_TEXTURE; }
	vec3 value(double& u, double& v, const vec3& p) const override
	{
		int x = int(floor(inv_scale * p.x));
		int y = int(floor(inv_scale * p.y));
		int z = int(floor(inv_scale * p.z));
		bool isEven = (x + y + z) % 2 == 0;
		return (isEven ? even : odd) -> value(u, v, p);
	}

//private:
	double inv_scale;
	shared_ptr<texture> even;
	shared_ptr<texture> odd;
};

class image_texture : public texture
{
public:
	image_texture(const string& filename)
	{
		_image.loadFromFile(filename);
		return;
	}
	inline RAY_TRACING_ELEMENT type() const override { return TEXTURE_IMAGE_TEXTURE; }
	vec3 value(double& u, double& v, const vec3& p) const override
	{
		if (_image.size().x <= 0) return vec3(0, 1, 1);
		u = interval(0, 1).clamp(u);
		v = 1.0 - interval(0, 1).clamp(v);

		int i = u * _image.size().x;
		int j = v * _image.size().y;
		return xyz(_image.pixel(i, j));
	}


//private:
	image _image;
};