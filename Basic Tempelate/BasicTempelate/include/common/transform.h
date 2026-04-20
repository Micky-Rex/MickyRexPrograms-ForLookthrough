#pragma once
#include "interval.h"
#include "ray_tracing/ray.h"
//#include "Basic.h"

struct transform
{
public:
	transform(
		const vec3& pos = vec3(0, 0, 0),
		const quat& rot = quat(0, 0, 0, 0),
		const vec3& scl = vec3(1, 1, 1))
	{
		translation = pos;
		rotation = rot;
		scale = scl;
		transform_dirty = true;
	}

	vec3 transform_point(const vec3& p) const
	{
		update_rotation();
		vec3 scaled = p * scale;
		return rotation_matrix_fwd * scaled + translation;
	}

	vec3 inverse_transform_point(const vec3& p) const
	{
		update_rotation();
		vec3 untranslated = p - translation;
		return rotation_matrix_inv * untranslated / scale;
	}

	vec3 transform_vec(const vec3& v) const
	{
		update_rotation();
		vec3 scaled = v * scale;
		return rotation_matrix_fwd * scaled;
	}

	vec3 inverse_transform_vec(const vec3& v) const
	{
		update_rotation();
		return rotation_matrix_inv * v / scale;
	}

	ray local_ray(const ray& r) const
	{
		return ray(inverse_transform_point(r.original),
			inverse_transform_vec(r.direction));
	}

	void set_position(const vec3& pos)
	{
		translation = pos;
		transform_dirty = true;
	}

	void set_scale(const vec3& scl)
	{
		scale = scl;
		transform_dirty = true;
	}

	void set_rotation(const vec3& euler)
	{
		rotation = normalize(quat(euler));
		transform_dirty = true;
	}

	void set_rotation(const quat& q)
	{
		rotation = normalize(q);
		transform_dirty = true;
	}

	void rotate_x(const double& angle_rad)
	{
		rotation = normalize(quat(vec3(angle_rad, 0, 0)) * rotation);
		transform_dirty = true;
	}

	void rotate_y(const double& angle_rad)
	{
		rotation = normalize(quat(vec3(0, angle_rad, 0)) * rotation);
		transform_dirty = true;
	}

	void rotate_z(const double& angle_rad)
	{
		rotation = normalize(quat(vec3(0, 0, angle_rad)) * rotation);
		transform_dirty = true;
	}

	void rotate_axis(const double& angle_rad, const vec3& axis)
	{
		rotation = normalize(glm::angleAxis(static_cast<double>(angle_rad), axis) * rotation);
		transform_dirty = true;
	}

	vec3 get_eular_angles() const
	{
		return eulerAngles(rotation);
	}

	quat get_rotation() const
	{
		return rotation;
	}

	vec3 get_position() const { return translation; }
	vec3 get_scale() const { return scale; }

	// 新增：获取完整变换矩阵
	mat4 get_transform_matrix() const
	{
		mat4 translation_mat = glm::translate(mat4(1.0), glm::vec3(translation));
		mat4 rotation_mat = mat4_cast(rotation);
		mat4 scale_mat = glm::scale(mat4(1.0), glm::vec3(scale));
		return translation_mat * rotation_mat * scale_mat;
	}

	mat4 get_inverse_matrix(const mat4& m) const
	{
		float aug[4][8]; // 增广矩阵：4行8列

		// 初始化增广矩阵：左半部分为原矩阵，右半部分为单位矩阵
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				aug[i][j] = m[j][i];    // 注意：GLM 是列优先存储，需转置为行优先
				aug[i][j + 4] = (i == j) ? 1.0f : 0.0f;
			}
		}

		// 高斯-约当消元
		for (int k = 0; k < 4; ++k) {
			// 选主元：在列 k 中找绝对值最大的行
			int r = k;
			float max_val = std::fabs(aug[k][k]);
			for (int i = k + 1; i < 4; ++i) {
				if (std::fabs(aug[i][k]) > max_val) {
					max_val = std::fabs(aug[i][k]);
					r = i;
				}
			}

			// 交换行 k 和行 r
			if (r != k) {
				for (int j = 0; j < 8; ++j) {
					std::swap(aug[k][j], aug[r][j]);
				}
			}

			// 归一化主元行
			float pivot = aug[k][k];
			if (interval(-1e-4, 1e-4).contains(pivot)) {
				throw std::runtime_error("Matrix is singular (not invertible)");
			}
			for (int j = 0; j < 8; ++j) {
				aug[k][j] /= pivot;
			}

			// 消去其他行
			for (int i = 0; i < 4; ++i) {
				if (i == k) continue;
				float factor = aug[i][k];
				for (int j = 0; j < 8; ++j) {
					aug[i][j] -= factor * aug[k][j];
				}
			}
		}

		// 提取逆矩阵（增广矩阵右半部分）
		glm::mat4 inv(0.0f);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				inv[j][i] = aug[i][j + 4]; // 转置回列优先存储
			}
		}
		return inv;
	}

private:
	vec3 translation;
	quat rotation = quat();
	vec3 scale = vec3(1, 1, 1);

	mutable mat3 rotation_matrix_fwd;  // 正向旋转矩阵
	mutable mat3 rotation_matrix_inv;  // 逆旋转矩阵
	mutable bool transform_dirty = true;

	void update_rotation() const
	{
		if (!transform_dirty) return;

		// 使用 GLM 将四元数转换为旋转矩阵
		rotation_matrix_fwd = mat3_cast(rotation);

		// 计算逆旋转矩阵（转置矩阵，因为旋转矩阵是正交矩阵）
		rotation_matrix_inv = transpose(rotation_matrix_fwd);

		transform_dirty = false;
	}
};
/*
struct transform
{
public:
	transform(
		const vec3& pos = vec3(0, 0, 0),
		const quat& rot = quat(),
		const vec3& scl = vec3(1, 1, 1))
	{
		translation = pos; rotation = rot; scale = scl;
		transform_dirty = true;
	}

	vec3 transform_point(const vec3& p) const
	{
		update_rotation();
		vec3 scaled = p * scale;
		// 正确的矩阵乘法（行矩阵乘列向量）
		return vec3(
			rotation_matrix_fwd[0].x * scaled.x + rotation_matrix_fwd[0].y * scaled.y + rotation_matrix_fwd[0].z * scaled.z,
			rotation_matrix_fwd[1].x * scaled.x + rotation_matrix_fwd[1].y * scaled.y + rotation_matrix_fwd[1].z * scaled.z,
			rotation_matrix_fwd[2].x * scaled.x + rotation_matrix_fwd[2].y * scaled.y + rotation_matrix_fwd[2].z * scaled.z
		) + translation;
	}

	vec3 inverse_transform_point(const vec3& p) const
	{
		update_rotation();
		vec3 untranslated = p - translation;
		// 直接使用缓存的逆旋转矩阵（无需转置操作）
		return vec3(
			dot(rotation_matrix_inv[0], untranslated),
			dot(rotation_matrix_inv[1], untranslated),
			dot(rotation_matrix_inv[2], untranslated)
		) / scale;
	}

	vec3 transform_vec(const vec3& v) const
	{
		update_rotation();
		vec3 scaled = v * scale;
		// 正确的矩阵乘法
		return vec3(
			rotation_matrix_fwd[0].x * scaled.x + rotation_matrix_fwd[0].y * scaled.y + rotation_matrix_fwd[0].z * scaled.z,
			rotation_matrix_fwd[1].x * scaled.x + rotation_matrix_fwd[1].y * scaled.y + rotation_matrix_fwd[1].z * scaled.z,
			rotation_matrix_fwd[2].x * scaled.x + rotation_matrix_fwd[2].y * scaled.y + rotation_matrix_fwd[2].z * scaled.z
		);
	}

	vec3 inverse_transform_vec(const vec3& v) const
	{
		update_rotation();
		// 直接使用缓存的逆旋转矩阵
		return vec3(
			dot(rotation_matrix_inv[0], v),
			dot(rotation_matrix_inv[1], v),
			dot(rotation_matrix_inv[2], v)
		) / scale;
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
		rotation = normalize(quat(vec3(euler.x, euler.y, euler.z)));
		transform_dirty = true;
		return;
	}
	void set_rotation(const quat& q)
	{
		rotation = normalize(q);
		transform_dirty = true;
		return;
	}
	void rotate_x(const double& angle_rad)
	{
		quat rot = quat(vec3(angle_rad, 0, 0));
		rotation = (rot * rotation);
		transform_dirty = true;
		return;
	}
	void rotate_y(const double& angle_rad)
	{
		quat rot = quat(vec3(0, angle_rad, 0));
		rotation = normalize(rot * rotation);
		transform_dirty = true;
		return;
	}
	void rotate_z(const double& angle_rad)
	{
		quat rot = quat(vec3(0, 0, angle_rad));
		rotation = normalize(rot * rotation);
		transform_dirty = true;
		return;
	}
	void rotate_axis(const double& angle_rad, const vec3& axis)
	{
		vec3 norm_axis = normalize(axis);
		double s = sin(angle_rad / 2);
		quat rot(cos(angle_rad / 2), norm_axis.x * s, norm_axis.y * s, norm_axis.z * s);
		rotation = normalize(rot * rotation);
		transform_dirty = true;
		return;
	}

	vec3 get_eular_angles() const
	{
		return glm::eulerAngles(rotation);
	}

	quat get_rotation() const
	{
		return rotation;
	}

	vec3 get_position() const { return translation; }
	vec3 get_scale() const { return scale; }

private:
	vec3 translation;
	quat rotation = quat();
	vec3 scale = vec3(1, 1, 1);

	mutable vec3 rotation_matrix_fwd[3];  // 正向旋转矩阵
	mutable vec3 rotation_matrix_inv[3];  // 逆旋转矩阵（转置）
	mutable bool transform_dirty = true;

	void update_rotation() const
	{
		if (!transform_dirty) return;
		rotation.to_rotation_matrix(rotation_matrix_fwd);

		// 计算并缓存逆旋转矩阵（转置）
		rotation_matrix_inv[0] = vec3(rotation_matrix_fwd[0].x, rotation_matrix_fwd[1].x, rotation_matrix_fwd[2].x);
		rotation_matrix_inv[1] = vec3(rotation_matrix_fwd[0].y, rotation_matrix_fwd[1].y, rotation_matrix_fwd[2].y);
		rotation_matrix_inv[2] = vec3(rotation_matrix_fwd[0].z, rotation_matrix_fwd[1].z, rotation_matrix_fwd[2].z);

		transform_dirty = false;
	}
};
*/