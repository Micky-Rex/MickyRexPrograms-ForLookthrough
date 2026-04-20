#pragma once
#include "ray.h"
#include "../RayTracing.h"
#include "../interval.h"

namespace BasicRayTracing {
	class material;

	/* 光线击中的记录
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
	public:
		transform(
			const vec3& pos = vec3(0, 0, 0),
			const quaternion& rot = quaternion(),
			const vec3& scl = vec3(1, 1, 1))
		{
			translation = pos, rotation = rot, scale = scl;
			transform_dirty = true;
			return;
		}

		vec3 transform_point(const vec3& p) const
		{
			update_rotation();
			vec3 scaled = p * scale;
			// 修正：正确的矩阵乘法（矩阵乘以列向量）
			return vec3(
				rotation_matrix[0].x * scaled.x + rotation_matrix[0].y * scaled.y + rotation_matrix[0].z * scaled.z,
				rotation_matrix[1].x * scaled.x + rotation_matrix[1].y * scaled.y + rotation_matrix[1].z * scaled.z,
				rotation_matrix[2].x * scaled.x + rotation_matrix[2].y * scaled.y + rotation_matrix[2].z * scaled.z
			) + translation;
		}

		vec3 inverse_transform_point(const vec3& p) const
		{
			update_rotation();
			// 先移除平移
			vec3 untranslated = p - translation;

			// 计算逆旋转（旋转矩阵的转置）
			vec3 unrotated = vec3(
				rotation_matrix[0].x * untranslated.x + rotation_matrix[1].x * untranslated.y + rotation_matrix[2].x * untranslated.z,
				rotation_matrix[0].y * untranslated.x + rotation_matrix[1].y * untranslated.y + rotation_matrix[2].y * untranslated.z,
				rotation_matrix[0].z * untranslated.x + rotation_matrix[1].z * untranslated.y + rotation_matrix[2].z * untranslated.z
			);

			// 应用逆缩放
			return unrotated / scale;
		}

		vec3 transform_vec(const vec3& v) const
		{
			update_rotation();
			vec3 scaled = v * scale;
			// 修正：正确的矩阵乘法（矩阵乘以列向量）
			return vec3(
				rotation_matrix[0].x * scaled.x + rotation_matrix[0].y * scaled.y + rotation_matrix[0].z * scaled.z,
				rotation_matrix[1].x * scaled.x + rotation_matrix[1].y * scaled.y + rotation_matrix[1].z * scaled.z,
				rotation_matrix[2].x * scaled.x + rotation_matrix[2].y * scaled.y + rotation_matrix[2].z * scaled.z
			);
		}

		vec3 inverse_transform_vec(const vec3& v) const
		{
			update_rotation();
			// 逆向量变换：先逆旋转，再逆缩放
			vec3 unrotated = vec3(
				rotation_matrix[0].x * v.x + rotation_matrix[1].x * v.y + rotation_matrix[2].x * v.z,
				rotation_matrix[0].y * v.x + rotation_matrix[1].y * v.y + rotation_matrix[2].y * v.z,
				rotation_matrix[0].z * v.x + rotation_matrix[1].z * v.y + rotation_matrix[2].z * v.z
			);
			return unrotated / scale;
		}

		ray local_ray(const ray& r) const
		{
			return ray(inverse_transform_point(r.original), inverse_transform_vec(r.direction));
		}

		void set_position(const vec3& pos)
		{
			translation = pos;
			transform_dirty = true;
			return;
		}

		void set_scale(const vec3& scl)
		{
			scale = scl;
			transform_dirty = true;
			return;
		}

		void set_rotation(const vec3& euler)
		{
			rotation = quaternion::from_euler(euler.x, euler.y, euler.z).normalized();
			transform_dirty = true;
			return;
		}

		void set_rotation(const quaternion& q)
		{
			rotation = q.normalized();
			transform_dirty = true;
			return;
		}

		void rotate_x(const double& angle_rad)
		{
			quaternion rot = quaternion::from_euler(angle_rad, 0, 0);
			rotation = (rot * rotation).normalized();
			transform_dirty = true;
			return;
		}
		void rotate_y(const double& angle_rad)
		{
			quaternion rot = quaternion::from_euler(0, angle_rad, 0);
			rotation = (rot * rotation).normalized();
			transform_dirty = true;
			return;
		}
		void rotate_z(const double& angle_rad)
		{
			quaternion rot = quaternion::from_euler(0, 0, angle_rad);
			rotation = (rot * rotation).normalized();
			transform_dirty = true;
			return;
		}
		void rotate_axis(const double& angle_rad, const vec3& axis)
		{
			vec3 norm_axis = normalize(axis);
			double s = sin(angle_rad / 2);
			quaternion rot(cos(angle_rad / 2), norm_axis.x * s, norm_axis.y * s, norm_axis.z * s);
			rotation = (rot * rotation).normalized();
			transform_dirty = true;
			return;
		}

		vec3 get_eular_angles() const
		{
			return rotation.to_euler();
		}

		quaternion get_rotation() const
		{
			return rotation;
		}

		vec3 get_position() const { return translation; }
		vec3 get_scale() const { return scale; }

	private:
		vec3 translation;
		quaternion rotation = quaternion();
		vec3 scale = vec3(1, 1, 1);

		mutable vec3 rotation_matrix[3];
		mutable bool transform_dirty = true;

		void update_rotation() const
		{
			if (!transform_dirty) return;
			rotation.to_rotation_matrix(rotation_matrix);
			transform_dirty = false;
			return;
		}

	};

	/*
	* 所有可被光线击中的基类
	*/
	class hittable
	{
	public:
		transform transformation;
		virtual bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const = 0;
		virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const
		{
			bool returned = hit_located_ray(transformation.local_ray(r), ray_t, rec);
			rec.p = transformation.transform_point(rec.p);
			rec.normal = transformation.transform_vec(rec.normal);
			return returned;
		}
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
		sphere(const vec3& _center, double radius, shared_ptr<material> mat) : center(), radius(radius), mat(mat) { transformation = transform(_center); };
		inline bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const override
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
		void set_radius(const double& _radius) { radius = _radius; }
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
		vec3 outward_normal;
	public:
		triangle(const vec3& p1, const vec3& p2, const vec3& p3, shared_ptr<material> mat) : p1(p1), p2(p2), p3(p3), mat(mat), e1(p2 - p1), e2(p3 - p1) { outward_normal = normalize(cross(e2, e1)); };

		// 基于 Moller-Trumbore 算法
		inline bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const override
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
	* 3D模型
	*/
	class model : public hittable
	{
	public:
		vector<triangle> triangles;
		virtual bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const
		{
			hit_record temp_rec;
			bool hit_anything = false;
			double closest_so_far = ray_t.max;

			for (const auto& obj : triangles)
			{
				if (obj.hit(r, interval(ray_t.min, closest_so_far), temp_rec))
				{
					hit_anything = true;
					closest_so_far = temp_rec.t;
					rec = temp_rec;
				}
			}
			return hit_anything;
		}
		inline void add(shared_ptr<triangle> obj)
		{
			triangles.push_back(*obj);
			return;
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

		inline bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const override
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