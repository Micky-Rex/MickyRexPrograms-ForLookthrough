#pragma once
#include "omp.h"
#include "material.h"
#include "interval.h"
#include "hittable.h"
#include "Graphics.h"

namespace BasicRayTracing2D {

	struct vec4
	{
		vec3 xyz;
		double x, y, z, a;
		vec4() : x(0), y(0), z(0), a(0) {};
		vec4(vec3 v, double _a)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			a = _a;
			xyz = v;
			return;
		}
		vec4(double x, double y, double z, double a) : x(x), y(y), z(z), a(z), xyz(vec3(x, y, z)) {};
		vec4 operator + (const vec4& vec)
		{
			xyz = vec3(x, y, z);
			return vec4(x + vec.x, y + vec.y, z + vec.z, a + vec.a);
		}
		vec4 operator / (const vec4& vec)
		{
			return vec4(x / vec.x, y / vec.y, z / vec.z, a / vec.a);
		}
	};
	struct result_image
	{
		vec4* data;
		unsigned int frame_number; // Ö¡µþ¼ÓµÄ²ãÊý
		unsigned int width, height;
		result_image() { width = 0, height = 0; data = new vec4[0]; frame_number = 1; };
		result_image(const result_image& image)
		{
			*this = image;
			frame_number = image.frame_number;
			return;
		}
		result_image(unsigned int width, unsigned int height): width(width), height(height)
		{
			data = new vec4[width * height];
			frame_number = 1;
			return;
		}
		void operator = (const result_image& other_image)
		{
			width = other_image.width;
			height = other_image.height;
			delete[] data;
			data = new vec4[width * height];
			memcpy(data, other_image.data, width * height * sizeof(vec4));
			//printf("1");
			//#pragma omp parallel for
			//for (int i = 0; i < width * height; i++)
			//{
			//	data[i] = other_image.data[i];
			//	//printf("%f, %f, %f\n", data[i].x, data[i].y, data[i].z);
			//}
			frame_number = other_image.frame_number;
			return;
		}
		result_image operator + (const result_image& other_image)
		{
			if (this->width != other_image.width
				or this->height != other_image.height)
			{
				printf("Failed to superposition two images!\n");
				return result_image();
			}
			result_image result = result_image(width, height);

			memcpy(data, other_image.data, width * height * sizeof(vec4));
			//#pragma omp parallel for
			//for (int i = 0; i < width * height; i++)
			//{
			//	result.data[i] = this->data[i] + other_image.data[i];
			//}
			frame_number += other_image.frame_number;
			return result;
		}
		void operator += (const result_image& other_image)
		{
			if (this->width != other_image.width
				or this->height != other_image.height)
			{
				printf("Failed to superposition two images!\n");
				return;
			}

			#pragma omp parallel for
			for (int i = 0; i < width * height; i++)
			{
				data[i] = data[i] + other_image.data[i];
			}
			frame_number += other_image.frame_number;
			return;
		}
		~result_image()
		{
			delete[] data;
			return;
		}
	};

	inline Color vec_to_color(const vec3& vec)
	{
		return Color(int(vec.x * 255), int(vec.y * 255), int(vec.z * 255));
	}
	class camera
	{
	public:
		int image_width;
		double aspect_ratio;
		int samples_per_pixel;
		int max_depth;
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

//		Image render(const hittable& world)
//		{
//			init();
//
//			Image texture;
//			texture.create(image_width, image_height);
//
//			omp_set_num_threads(std::max(omp_get_num_procs() - 1, 1));
//#pragma omp parallel for
//			for (int j = 0; j < window_height; j++)
//			{
//				for (int i = 0; i < window_width; i++)
//				{
//					vec3 pixel_color = vec3(0, 0, 0);
//					for (int sample = 0; sample < samples_per_pixel; sample++)
//					{
//						ray r = get_ray(i, j);
//						pixel_color += ray_color(r, world, max_depth);
//					}
//
//					texture.setPixel(i, j, vec_to_color(linear_to_gamma(pixel_color * pixel_samples_scale)));
//				}
//			}
//			return texture;
//		}	

		result_image render(const hittable& world)
		{
			init();

			result_image texture(window_width, window_height);

			omp_set_num_threads(std::max(omp_get_num_procs() - 1, 1));
			#pragma omp	parallel for
			for (int j = 0; j < window_height; j++)
			{
				for (int i = 0; i < window_width; i++)
				{
					vec3 pixel_color = vec3(0, 0, 0);
					for (int sample = 0; sample < samples_per_pixel; sample++)
					{
						ray r = get_ray(i, j);
						pixel_color += ray_color(r, world, max_depth);
					}

					//printf("%d\n", i * window_width + j);
					texture.data[j * window_width + i] = vec4(linear_to_gamma(pixel_color * pixel_samples_scale), 0);
				}
			}
			return texture;
		}
	private:
		vec3 center;
		int image_height;
		vec3 pixel_delta_u;
		vec3 pixel_delta_v;
		vec3 pixel00_center;
		double pixel_samples_scale;
		vec3 u, v, w;

		void init()
		{
			this->image_height = int(window_width / aspect_ratio);

			pixel_samples_scale = 1.f / samples_per_pixel;

			center = lookfrom;

			double focal_length = get_length(lookfrom - lookat);
			double theta = radians(fov);
			double h = tan(theta / 2.0);

			double viewport_height = 2.f * h * focal_length;
			double viewport_width = viewport_height * (double(window_width) / window_height);

			w = normalize(lookfrom - lookat);
			u = normalize(cross(vup, w));
			v = cross(w, u);

			vec3 viewport_u = viewport_width * u;
			vec3 viewport_v = viewport_height * -v;

			pixel_delta_u = viewport_u / double(window_width);
			pixel_delta_v = viewport_v / double(window_height);

			vec3 viewport_upper_left = center - (focal_length * w) - viewport_u / 2.f - viewport_v / 2.f;
			pixel00_center = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);
			return;
		}
		ray get_ray(int i, int j) const
		{
			vec3 offset = sample_square();
			vec3 pixel_sample = pixel00_center + (double(i + offset.x) * pixel_delta_u) + (double(j + offset.y) * pixel_delta_v);

			vec3 ray_origin = center;// +randomvec3(-0.1, 0.1);
			vec3 ray_direction = pixel_sample - ray_origin + randomvec3(-1e-4, 1e-4);

			return ray(ray_origin, ray_direction);
		}
		vec3 ray_color(const ray& r, const hittable& world, int depth) const
		{
			if (depth <= 0) return vec3(0, 0, 0);
			hit_record rec;
			if (world.hit(r, interval(1e-3f, infinity), rec))
			{
				ray scattered;
				vec3 attenuation;
				if (rec.mat->scatter(r, rec, attenuation, scattered))
				{
					return attenuation * ray_color(scattered, world, depth - 1);
				}
				return vec3(0, 0, 0);
			}
			double a = 0.5f * (normalize(r.direction).y + 1.f);
			return (1.f - a) * vec3(1.f, 1.f, 1.f) + a * vec3(0.5f, 0.7f, 1.f);
			//return vec3(1, 1, 1);
		}
		vec3 sample_square() const
		{
			return vec3(randomf() - 0.5f, randomf() - 0.5f, 0);
		}
	};
}