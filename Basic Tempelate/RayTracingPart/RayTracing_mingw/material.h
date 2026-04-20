#pragma once
#include "hittable.h"

namespace BasicRayTracing2D {
	class material
	{
	public:
		virtual ~material() = default;
		virtual bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const
		{
			return false;
		}
	};
	class lambertian : public material
	{
	private:
		vec3 albedo;
	public:
		lambertian(const vec3 albedo) : albedo(albedo) {};
		bool scatter(const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered) const override
		{
			vec3 scatter_direction = rec.normal + random_unitvec3();
			if (get_length(scatter_direction) <= 1e-50) scatter_direction = rec.normal;
			scattered = ray(rec.p, scatter_direction);
			attenuation = albedo;
			return true;
		}
	};
	class metal : public material
	{
	private:
		vec3 albedo;
		double fuzz;
	public:
		metal(const vec3 albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {};
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
	private:
		double refraction_index;
		static double reflectance(double cosine, double _refraction_index)
		{
			double r0 = (1 - _refraction_index) / (1 + _refraction_index);
			r0 = r0 * r0;
			return r0 + (1 - r0) * pow((1 - cosine), 5);
		}
	public:
		dielectric(double refraction_index) : refraction_index(refraction_index) {};
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
	};
}