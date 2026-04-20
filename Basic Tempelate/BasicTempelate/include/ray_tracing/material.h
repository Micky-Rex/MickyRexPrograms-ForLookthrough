#pragma once
#include "ray.h"
#include "texture.h"
/* 光线击中的记录
* vec3 p 交点
* vec3 normal 交点处法线
* double t 二次方程的根（光线从原点到交点的模长）
* bool front_face 是否是外面的面
*/

class material;
struct hit_record
{
public:
	vec3 p;
	vec3 normal;
	shared_ptr<material> mat;
	double t;
	double u, v;
	bool front_face;
	inline void set_face_normal(const ray& r, const vec3& outward_normal)
	{
		front_face = dot(r.direction, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
		return;
	}
};
class material
{
public:
	virtual ~material() = default;
	virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		return false;
	}
	virtual RAY_TRACING_ELEMENT type() const = 0;
	virtual vec3 emitted(double& u, double& v, const vec3& p) const
	{
		return vec3(0,0,0);
	}
};
class lambertian : public material
{
public:
//private:
	shared_ptr<texture> tex;
//public:
	lambertian(shared_ptr<texture> tex) : tex(tex) {};
	lambertian(const vec3& albedo) : tex(make_shared<solid_color>(albedo)) {};
	inline RAY_TRACING_ELEMENT type() const override { return MATERIAL_LAMBERTIAN; }
	bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override
	{
		vec3 scatter_direction = rec.normal + random_unitvec3();
		if (get_length(scatter_direction) <= 1e-50) scatter_direction = rec.normal;
		scattered = ray(rec.p, scatter_direction);
		attenuation = tex->value(rec.u, rec.v, rec.p);
		return true;
	}
};
class metal : public material
{
public:
	vec3 albedo;
	double fuzz;
//public:
	metal(const vec3 albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {};
	inline RAY_TRACING_ELEMENT type() const override { return MATERIAL_METAL; }
	bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override
	{
		vec3 reflected = reflect(rec.normal, r_in.direction);
		reflected = normalize(reflected) + (fuzz * random_unitvec3());
		scattered = ray(rec.p, reflected);
		attenuation = albedo;
		return (dot(scattered.direction, rec.normal) > 0);
	}
};
class dielectric : public material
{
public:
	double refraction_index;
	static double reflectance(double cosine, double _refraction_index)
	{
		double r0 = (1 - _refraction_index) / (1 + _refraction_index);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
//public:
	dielectric(double refraction_index) : refraction_index(refraction_index) {};
	inline RAY_TRACING_ELEMENT type() const override { return MATERIAL_DIELECTRIC; }
	bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override
	{
		attenuation = vec3(1, 1, 1);
		double ri = rec.front_face ? (1.f / refraction_index) : refraction_index;

		vec3 unit_direction = normalize(r_in.direction);
		double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.f);
		double sin_theta = sqrt(1.f - cos_theta * cos_theta);

		bool cannot_refract = ri * sin_theta > 1.0;
		vec3 direction;

		if (cannot_refract or reflectance(cos_theta, ri) > randomf())
			direction = reflect(rec.normal, unit_direction);
		else
			direction = refract(unit_direction, rec.normal, ri);

		scattered = ray(rec.p, direction);
		return true;
	}
};
class dissuse_light : public material
{
public:
	dissuse_light(shared_ptr<texture> tex) : tex(tex) {}
	dissuse_light(const vec3& emit) : tex(make_shared<solid_color>(emit)) {}

	inline RAY_TRACING_ELEMENT type() const override { return MATERIAL_DISSUSE_LIGHT; }
	vec3 emitted(double& u, double& v, const vec3& p) const override
	{
		return tex->value(u, v, p);
	}

//private:
	shared_ptr<texture> tex;
};