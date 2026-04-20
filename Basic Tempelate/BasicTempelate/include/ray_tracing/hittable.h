#pragma once
#include "ray.h"
#include "material.h"
#include "../common/interval.h"
#include "obj_loader.h"

class material;

/*
* AABB 包围盒
*/
struct aabb
{
public:
	interval x, y, z;//	在三个坐标上的投影范围
	aabb() {}
	aabb(const interval& x, const interval& y, const interval& z) : x(x), y(y), z(z) { pad_to_minimums(); }
	aabb(const vec3& a, const vec3& b)	// 左下前点和右上后点
	{
		x = (a.x <= b.x) ? interval(a.x, b.x) : interval(b.x, a.x);
		y = (a.y <= b.y) ? interval(a.y, b.y) : interval(b.y, a.y);
		z = (a.z <= b.z) ? interval(a.z, b.z) : interval(b.z, a.z);
		pad_to_minimums();
		return;
	}
	aabb(const aabb& box1, const aabb& box2)
	{
		x = interval(box1.x, box2.x);
		y = interval(box1.y, box2.y);
		z = interval(box1.z, box2.z);
		pad_to_minimums();
		return;
	}

	virtual bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const
	{
		const double adinv_x = 1.0 / r.direction.x;
		const double adinv_y = 1.0 / r.direction.y;
		const double adinv_z = 1.0 / r.direction.z;

		const double t0x = (x.min - r.original.x) * adinv_x;
		const double t1x = (x.max - r.original.x) * adinv_x;
		const double t0y = (y.min - r.original.y) * adinv_y;
		const double t1y = (y.max - r.original.y) * adinv_y;
		const double t0z = (z.min - r.original.z) * adinv_z;
		const double t1z = (z.max - r.original.z) * adinv_z;

		if (t0x < t1x) {
			if (t0x > ray_t.min) ray_t.min = t0x;
			if (t1x < ray_t.max) ray_t.max = t1x;
		}
		else {
			if (t1x > ray_t.min) ray_t.min = t1x;
			if (t0x < ray_t.max) ray_t.max = t0x;
		}
		if (ray_t.max <= ray_t.min) return false;

		if (t0y < t1y) {
			if (t0y > ray_t.min) ray_t.min = t0y;
			if (t1y < ray_t.max) ray_t.max = t1y;
		}
		else {
			if (t1y > ray_t.min) ray_t.min = t1y;
			if (t0y < ray_t.max) ray_t.max = t0y;
		}
		if (ray_t.max <= ray_t.min) return false;

		if (t0z < t1z) {
			if (t0z > ray_t.min) ray_t.min = t0z;
			if (t1z < ray_t.max) ray_t.max = t1z;
		}
		else {
			if (t1z > ray_t.min) ray_t.min = t1z;
			if (t0z < ray_t.max) ray_t.max = t0z;
		}
		if (ray_t.max <= ray_t.min) return false;

		return true;
	}
	bool operator == (const aabb& box)
	{
		return x == box.x and y == box.y and z == box.z;
	}
	bool operator != (const aabb& box)
	{
		return !(*this == box);
	}

private:
	void pad_to_minimums()
	{
		const double delta = 1e-4;
		if (x.size() < delta) x = x.expand(delta);
		if (y.size() < delta) y = y.expand(delta);
		if (z.size() < delta) z = z.expand(delta);
		return;
	}
};
/*
* 所有可被光线击中的基类
*/
class hittable
{
public:
	aabb bbox;
	shared_ptr<material> mat;
	// struct transform transformation;
	mat4 transformation, inversed_trans;
	virtual bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const = 0;
	virtual RAY_TRACING_ELEMENT type() const = 0;
	virtual void init_bounding_box() = 0;
	bool hit(const ray& r, interval ray_t, hit_record& rec) const
	{
		ray located = r;
		if (inversed_trans != mat4(0))
		{
			located.original = vec4(located.original, 1.f) * inversed_trans;
			located.direction = vec4(located.direction, 0.f) * inversed_trans;
		}
		bool returned = bbox.hit_located_ray(located, ray_t, rec);
		if (returned == false) return false;
		returned = hit_located_ray(located, ray_t, rec);
		rec.p = vec4(rec.p, 1.f) * transformation;
		rec.normal = normalize(glm::transpose(inversed_trans) * vec4(rec.normal, 0.f));
		return returned;
	}
	void update_inversed() { inversed_trans = glm::inverse(transformation); }
	void translate(const vec3& v) { transformation = glm::translate(transformation, glm::vec3(v)); update_inversed(); }
	void rotate(const float _degress, const vec3& _axis) {transformation = glm::rotate(transformation, (float)degress(_degress), glm::vec3(_axis)); update_inversed(); }
	void rotate_x(const float _degress) { this->rotate(_degress, vec3(1, 0, 0)); }
	void rotate_y(const float _degress) { this->rotate(_degress, vec3(0, 1, 0)); }
	void rotate_z(const float _degress) { this->rotate(_degress, vec3(0, 0, 1)); }
	void scale(const vec3& _factor) { transformation = glm::scale(transformation, glm::vec3(_factor)); update_inversed(); }
	void set_position(const vec3& v)
	{
		transformation[3][0] = 0;
		transformation[3][1] = 0;
		transformation[3][2] = 0;
		this->translate(v);
	}
	void set_rotation(const vec3& v)
	{
		glm::vec3 scale;
		scale.x = glm::length(glm::vec3(transformation[0]));
		scale.y = glm::length(glm::vec3(transformation[1]));
		scale.z = glm::length(glm::vec3(transformation[2]));
		glm::vec3 translation = glm::vec3(transformation[3]);
		mat4 notrotated = mat4(1.f);
		notrotated = glm::scale(notrotated, scale);
		notrotated[3] = glm::vec4(translation, 1.f);
		transformation = notrotated;
		update_inversed();
	}
	void set_scale(const vec3& v)
	{
		transformation[0] = glm::normalize(transformation[0]);
		transformation[1] = glm::normalize(transformation[1]);
		transformation[2] = glm::normalize(transformation[2]);
		update_inversed();
	}
};

/*
* 球体
*/
class sphere : public hittable
{
public:
	vec3 center;
	double radius;
	static void get_uv(const vec3& p, double& u, double& v)
	{
		double theta = acos(-p.y);
		double phi = atan2(-p.z, p.x) + M_PI;
		u = phi / (2 * M_PI);
		v = theta / M_PI;
	}
//public:
	inline void init_bounding_box() override
	{
		vec3 rvec(radius, radius, radius);
		bbox = aabb(center - rvec, center + rvec);
	}
	sphere(const vec3& _center, double radius, shared_ptr<material> _mat) : center(), radius(radius) {
		mat = _mat;
		this->set_position(_center);
		//inversed_trans = glm::inverse(transformation);
		init_bounding_box();
	};
	inline RAY_TRACING_ELEMENT type() const override { return HITTABLE_SPHERE; }
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
		get_uv(outward_normal, rec.u, rec.v);
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
public:
	vec3 p1, p2, p3;
	//shared_ptr<material> mat;

	// 三角形两边的向量
	vec3 e1, e2;
	// 三角形法线
	vec3 outward_normal;
//public:
	inline void init_bounding_box() override
	{
		bbox = aabb(
			vec3(min(min(p1.x, p2.x), p3.x), min(min(p1.y, p2.y), p3.y), min(min(p1.z, p2.z), p3.z)),
			vec3(max(max(p1.x, p2.x), p3.x), max(max(p1.y, p2.y), p3.y), max(max(p1.z, p2.z), p3.z))
		);
		return;
	}
	triangle(const vec3& p1, const vec3& p2, const vec3& p3, shared_ptr<material> _mat) : p1(p1), p2(p2), p3(p3), e1(p2 - p1), e2(p3 - p1) { mat = _mat; outward_normal = normalize(cross(e2, e1)); init_bounding_box(); }
	inline RAY_TRACING_ELEMENT type() const override { return HITTABLE_TRIANGLE; }
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
* 平行四边形
*/
class quad : public hittable
{
public:
	inline void init_bounding_box() override
	{
		bbox = aabb(aabb(Q, Q + u + v), aabb(Q + u, Q + v));
		return;
	}
	quad(const vec3& Q, const vec3& u, const vec3& v, shared_ptr<material> _mat) : Q(Q), u(u), v(v)
	{
		mat = _mat;
		vec3 n = cross(u, v);
		normal = normalize(n);
		D = dot(normal, Q);
		w = n / dot(n, n);
		init_bounding_box();
		return;
	}
	inline RAY_TRACING_ELEMENT type() const override { return HITTABLE_QUAD; }

	bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const override
	{
		double denom = dot(normal, r.direction);
		if (fabs(denom) < 1e-8)
			return false;

		double t = (D - dot(normal, r.original)) / denom;
		if (!ray_t.contains(t))
			return false;

		vec3 intersection = r.at(t);
		vec3 planar_hitpt_vec = intersection - Q;
		double alpha = dot(w, cross(planar_hitpt_vec, v));
		double beta = dot(w, cross(u, planar_hitpt_vec));

		if (!is_interior(alpha, beta, rec))
			return false;
			
		rec.t = t;
		rec.p = intersection;
		rec.mat = mat;
		rec.set_face_normal(r, normal);
		return true;
	}
//private:
	vec3 Q;
	vec3 u, v, w;
	vec3 normal;
	double D;
	//shared_ptr<material> mat;
	virtual bool is_interior(const double& a, const double& b, hit_record& rec) const
	{
		interval unit_interval = interval(0, 1);
		if (!unit_interval.contains(a) or !unit_interval.contains(b))
			return false;
		rec.u = a;
		rec.v = b;
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
	virtual void init_bounding_box()
	{
		bbox = aabb(vec3(infinity, infinity, infinity), -vec3(infinity, infinity, infinity));
		return;
	}
	hittable_list() {};
	hittable_list(shared_ptr<hittable> obj) { add(obj); init_bounding_box(); }
	inline void clear() { objects.clear(); init_bounding_box(); }
	inline void add(shared_ptr<hittable> obj)
	{
		objects.push_back(obj);
		return;
	}
	inline RAY_TRACING_ELEMENT type() const override { return HITTABLE_LIST; }
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


/*
* 3D模型
*/
class model : public hittable_list
{
public:
	model() = delete;
	model(const OBJLoader::Model& mod)
	{
		model& _model = *this;
		shared_ptr<material> material_pot = make_shared<lambertian>(vec3(0.8, 0.8, 0.8));

		int cur = 0;
		while (!mod.triangles.empty() and cur < mod.triangles.size())
		{
			_model.add(make_shared<triangle>(
				mod.vertices[mod.triangles[cur].vertexIndices[0]],
				mod.vertices[mod.triangles[cur].vertexIndices[1]],
				mod.vertices[mod.triangles[cur].vertexIndices[2]],
				material_pot)
			);
			cur++;
		}
		return;
	}
	model(const string& filepath)
	{
		OBJLoader::Model mod;
		mod = OBJLoader::loadOBJ(filepath);

		model& _model = *this;
		shared_ptr<material> material_pot = make_shared<lambertian>(vec3(0.8, 0.8, 0.8));

		int cur = 0;
		while (!mod.triangles.empty() and cur < mod.triangles.size())
		{
			_model.add(make_shared<triangle>(
				mod.vertices[mod.triangles[cur].vertexIndices[0]],
				mod.vertices[mod.triangles[cur].vertexIndices[1]],
				mod.vertices[mod.triangles[cur].vertexIndices[2]],
				material_pot)
			);
			cur++;
		}
		init_bounding_box();
	}
	inline RAY_TRACING_ELEMENT type() const override { return HITTABLE_MODEL; }
	//vector<triangle> triangles;
	virtual void init_bounding_box()
	{
		if (this->bbox != aabb()) return;
		vec3 _min(infinity, infinity, infinity), _max(-infinity, -infinity, -infinity);
		for (shared_ptr<hittable> t : objects)
		{
			_min.x = min(t->bbox.x.min, _min.x);
			_min.y = min(t->bbox.y.min, _min.y);
			_min.z = min(t->bbox.z.min, _min.z);
			_max.x = max(t->bbox.x.max, _max.x);
			_max.y = max(t->bbox.y.max, _max.y);
			_max.z = max(t->bbox.z.max, _max.z);
		}
		bbox = aabb(_min, _max);
		return;
	}
	virtual bool hit_located_ray(const ray& r, interval ray_t, hit_record& rec) const
	{
		hit_record temp_rec;
		bool hit_anything = false;
		double closest_so_far = ray_t.max;

		for (const shared_ptr<hittable> obj : objects)
		{
			if (obj->hit_located_ray(r, interval(ray_t.min, closest_so_far), temp_rec))
			{
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
		return hit_anything;
	}
};
