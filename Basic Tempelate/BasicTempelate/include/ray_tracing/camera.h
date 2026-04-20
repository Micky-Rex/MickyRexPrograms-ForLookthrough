#pragma once
#include <stack>
#include "omp.h"
#include "../Basic.h"
#include "material.h"
#include "../common/interval.h"
#include "hittable.h"

class camera
{
public:
	int image_width;
	double aspect_ratio;
	int samples_per_pixel;
	int max_depth;
	vec3 background;	// 背景颜色
	double fov;
	vec3 lookfrom;
	vec3 lookat;
	vec3 vup;
	camera(double _aspect_ratio, int _image_width)
	{
		this->aspect_ratio = _aspect_ratio;
		this->image_width = _image_width;
		return;
	}

	image render(const hittable& world)
	{
		image texture(image_width, image_height);

		omp_set_num_threads(std::max(omp_get_num_procs() - 1, 1));
		#pragma omp	parallel for
		for (int j = 0; j < image_height; j++)
		{
			for (int i = 0; i < image_width; i++)
			{
				vec3 pixel_color = vec3(0, 0, 0);
				for (int sample = 0; sample < samples_per_pixel; sample++)
				{
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, world, max_depth);
				}
				
				//printf("%d\n", i * window_width + j);
				texture.data[j * image_width + i] = vec4(linear_to_gamma(pixel_color * pixel_samples_scale), 0);
			}
		}
		return texture;
	}

	void init()
	{
		this->image_height = int(image_width / aspect_ratio);

		pixel_samples_scale = 1.f / samples_per_pixel;

		center = lookfrom;

		double focal_length = get_length(lookfrom - lookat);
		double theta = radians(fov);
		double h = tan(theta / 2.0);

		double viewport_height = 2.f * h * focal_length;
		double viewport_width = viewport_height * (double(image_width) / image_height);

		w = normalize(lookfrom - lookat);
		u = normalize(cross(vup, w));
		v = cross(w, u);

		vec3 viewport_u = viewport_width * u;
		vec3 viewport_v = viewport_height * -v;

		pixel_delta_u = viewport_u / double(image_width);
		pixel_delta_v = viewport_v / double(image_height);

		vec3 viewport_upper_left = center - (focal_length * w) - viewport_u / 2.f - viewport_v / 2.f;
		pixel00_center = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);
		return;
	}

	void init_for_gpu()
	{
		this->image_height = int(this->image_width / this->aspect_ratio);
		this->pixel_samples_scale = 1.f / this->samples_per_pixel;
		float focal_length = get_length(this->lookfrom - this->lookat);
		float theta = radians(this->fov);
		float h = tan(theta / 2.f);
		float viewport_height = 2.f * h * focal_length;
		float viewport_width = viewport_height * this->aspect_ratio;
		this->w = normalize(this->lookfrom - this->lookat);
		this->u = normalize(cross(this->vup, this->w));
		this->v = normalize(cross(this->w, this->u));
		vec3 viewport_u = viewport_width * this->u;
		vec3 viewport_v = viewport_height * this->v;
		this->pixel_delta_u = viewport_u / this->image_width;
		this->pixel_delta_v = viewport_v / this->image_height;
		vec3 viewport_upper_left = this->lookfrom - this->w * focal_length - viewport_u / 2.f - viewport_v / 2.f;
		this->pixel00_center = viewport_upper_left + this->pixel_delta_u * 0.5f + this->pixel_delta_v * 0.5f;
		return;
	}
//private:
	vec3 center;
	int image_height;
	vec3 pixel_delta_u;
	vec3 pixel_delta_v;
	vec3 pixel00_center;
	double pixel_samples_scale;
	vec3 u, v, w;

	ray get_ray(int i, int j) const
	{
		vec3 offset = sample_square();
		vec3 pixel_sample = pixel00_center + (double(i + offset.x) * pixel_delta_u) + (double(j + offset.y) * pixel_delta_v);

		vec3 ray_origin = center;// +randomvec3(-0.1, 0.1);
		vec3 ray_direction = pixel_sample - ray_origin + randomvec3(-1e-4, 1e-4);

		return ray(ray_origin, ray_direction);
	}

	vec3 ray_color(const ray& r, const hittable& world, int depth) const {
		vec3 current_attenuation(1.0f, 1.0f, 1.0f); // 累积衰减系数
		vec3 result_color(0.0f, 0.0f, 0.0f);        // 最终累积颜色
		ray current_ray = r;                         // 当前处理的射线
		int bounces = depth;                         // 剩余弹射次数

		while (bounces-- > 0) {
			hit_record rec;
			// 检查射线是否击中物体
			if (!world.hit(current_ray, interval(0.001, infinity), rec)) {
				result_color += current_attenuation * background; // 未命中则添加背景色
				break;
			}

			// 添加自发光贡献
			vec3 emitted = rec.mat->emitted(rec.u, rec.v, rec.p);
			result_color += current_attenuation * emitted;

			// 处理散射
			ray scattered;
			vec3 attenuation;
			if (!rec.mat->scatter(current_ray, rec, attenuation, scattered)) {
				break; // 无散射则终止路径
			}

			// 更新累积衰减和射线
			current_attenuation *= attenuation;
			current_ray = scattered;
		}

		return result_color;
	}
	vec3 sample_square() const
	{
		return vec3(randomf() - 0.5f, randomf() - 0.5f, 0);
	}
};