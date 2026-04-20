#pragma once
#include "../Basic.h"

namespace BasicRayTracing2D {
	class ray
	{
	public:
		vec3 original;
		vec3 direction;
		ray() = default;
		ray(vec3 origin, vec3 dir)
		{
			this->original = origin;
			this->direction = dir;
			return;
		}
		// 返回向量模长为t单位长度时顶点的坐标
		vec3 at(double t) const
		{
			return this->original + t * this->direction;
		}
	};
}