#pragma once
#include "ray.h"
#include "RayTracing.h"
#include "interval.h"

namespace BasicRayTracing2D {
	class material;
	/*
	* 光线击中的记录
	* vec3 p 交点
	* vec3 normal 交点处法线
	* double t 二次方程的根（光线从原点到交点的模长）
	* bool front_face 是否是外面的面
	*/
	struct hit_record
	{
	public:
		vec3 p;
		vec3 normal;
		shared_ptr<material> mat;
		double t;
		bool front_face;
		inline void set_face_normal(const ray& r, const vec3& outward_normal)
		{
			front_face = dot(r.direction, outward_normal) < 0;
			normal = front_face ? outward_normal : -outward_normal;
			return;
		}
	};

	struct transform
	{
		vec3 translation;
	};

	/*
	* 所有可被光线击中的基类
	*/
	class hittable
	{
	public:
		virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
	};

	/*
	* 球体
	*/
	class sphere : public hittable
	{
	private:
		vec3 center;
		double radius;
		shared_ptr<material> mat;
	public:
		sphere(const vec3& center, double radius, shared_ptr<material> mat) : center(center), radius(radius), mat(mat) {};
		inline bool hit(const ray& r, interval ray_t, hit_record& rec) const override
		{
			vec3 oc = center - r.original;
			double a = dot(r.direction, r.direction);
			double h = dot(r.direction, oc);
			double c = dot(oc, oc) - radius * radius;
			double discriminant = h * h - a * c;
			if (discriminant < 0) return false;

			double sqrtd = sqrt(discriminant);
			double root = (h - sqrtd) / a;
			if (!ray_t.surrounds(root))
			{
				root = (h + sqrtd) / a;
				if (!ray_t.surrounds(root)) return false;
			}
			rec.t = root;
			rec.p = r.at(rec.t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat = mat;
			return true;
		}
	};

	/*
	* 三角形
	*/
	class triangle : public hittable
	{
	private:
		vec3 p1, p2, p3;
		shared_ptr<material> mat;

		// 三角形两边的向量
		vec3 e1, e2;
		// 三角形法线
		vec3 outward_normal = normalize(cross(e1, e2));
	public:
		triangle(const vec3& p1, const vec3& p2, const vec3& p3, shared_ptr<material> mat) : p1(p1), p2(p2), p3(p3), mat(mat), e1(p2 - p1), e2(p3 - p1) {};

		// 基于 Moller-Trumbore 算法
		inline bool hit(const ray& r, interval ray_t, hit_record& rec) const override
		{
			// 计算行列式(射线方向与e2的叉乘)
			vec3 P = cross(r.direction, e2);
			double det = dot(e1, P);

			// 如果行列式接近0，则射线与三角形平行
			if (fabs(det) < 1e-6)
				return false;

			double inv_det = 1.0 / det;
			// 计算从射线源点到p1的向量
			vec3 T = r.original - p1;

			// 计算u参数并测试边界
			double u = dot(T, P) * inv_det;
			if (!interval(0, 1).contains(u))
				return false;

			// 准备计算v参数
			vec3 Q = cross(T, e1);

			// 计算v参数并测试边界
			double v = dot(r.direction, Q) * inv_det;
			if (v < 0.0 or u + v>1.0)
				return false;

			// 计算射线参数t
			double t = dot(e2, Q) * inv_det;

			// 检查t的范围
			if (!ray_t.contains(t))
				return false;

			rec.t = t;
			rec.p = r.at(t);
			rec.mat = mat;
			rec.set_face_normal(r, outward_normal);
			return true;
		}
	};

	/*
	* 实体(hittable)的列表
	*/
	class hittable_list : public hittable
	{
	public:
		vector<shared_ptr<hittable> > objects;
		hittable_list() {};
		hittable_list(shared_ptr<hittable> obj) { add(obj); }
		inline void clear() { objects.clear(); }
		inline void add(shared_ptr<hittable> obj)
		{
			objects.push_back(obj);
			return;
		}
		inline bool hit(const ray& r, interval ray_t, hit_record& rec) const override
		{
			hit_record temp_rec;
			bool hit_anything = false;
			double closest_so_far = ray_t.max;

			for (const auto& obj : objects)
			{
				if (obj->hit(r, interval(ray_t.min, closest_so_far), temp_rec))
				{
					hit_anything = true;
					closest_so_far = temp_rec.t;
					rec = temp_rec;
				}
			}
			return hit_anything;
		}
	};
}