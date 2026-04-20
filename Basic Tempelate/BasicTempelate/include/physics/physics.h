
#pragma once
#include <map>
#include <stack>
#include "Basic.h"
#include "../common/interval.h"

namespace Physics
{
	struct Point
	{
	public:
		vec3 gravity = vec3(0, 9.8, 0);
		vec3 p, v, a, F;	// 位置 速度 加速度 合力
		float m, Ek;	// 质量 动能
		void update(float dt /*时间变化量*/) {
			a = F / m;
			a += gravity;
			v += a * dt;
			p += v * dt;
			Ek = 0.5 * m * pow(get_length(v), 2);
			return;
		}

	private:
		//float dy_m;	// 动质量
	};

	enum TiesType
	{
		LINE,
		SPRING
	};

	struct _PointPair
	{
		shared_ptr<Point> a, b;
		bool have(shared_ptr<Point> p) const {
			if (this->a == p) return true;
			if (this->b == p) return true;
			return false;
		}
		bool operator == (const _PointPair& p) {
			if (this->a == p.a and this->b == p.b) return true;
			if (this->a == p.b and this->b == p.a) return true;
			return false;
		}
	};
	//struct _TieData
	//{
	//	float length;
	//	TiesType type;
	//};
	//struct Tie
	//{
	//	_TieData data;
	//	_PointPair points;
	//	const Point* a = points.a;
	//	const Point* b = points.b;
	//	const TiesType* type = &data.type;
	//	const float* length = &data.length;
	//	bool have(Point* p) {
	//		return points.have(p);
	//	}
	//};
	struct Tie
	{
		Tie() = delete;
		Tie(shared_ptr<Point> _a, shared_ptr<Point> _b) {
			points.a = _a;
			points.b = _b;
		}
		TiesType type;
		float k, length;
		_PointPair points;
		bool have(shared_ptr<Point> p) const {
			return points.have(p);
		}
		bool operator == (const Tie& p) {
			if (this->points == p.points and this->type == p.type) return true;
			return false;
		}
	};

	// 对两点间施加弹簧力
	void elasticity(const Tie& tie) {
		float dis = distance(tie.points.a->p, tie.points.b->p);
		vec3 F = normalize(tie.points.b->p - tie.points.a->p) * tie.k * (dis - tie.length);
		if (tie.type == LINE and dis - tie.length < 0) return;
		F *= 0.999999;
		tie.points.a->F += F; tie.points.b->F -= F;
		return;
	}

	struct System
	{

	private:

	public:
		System(const System& sys) = delete;
		System() { gravity = vec3(0, -9.8, 0); t = 0; };

		float E, t;	// 总能量 时间
		vec3 gravity;
		vector<shared_ptr<Point> > objs;	// 所有物体编号
		vector<Tie> ties;	// 物体之间的连接

		void add(shared_ptr<Point> dot) {
			objs.push_back(dot);
			return;
		}
		void add(const Tie& tie) {
			ties.push_back(tie);
			return;
		}

		void erase(shared_ptr<Point> dot) {
			vector<shared_ptr<Point> >::iterator it = find(objs.begin(), objs.end(), dot);
			if (it == objs.end()) return;
			objs.erase(it);
			auto hasPoint = [dot](const Tie& tie) {
				return tie.have(dot);
			};
			vector<Tie>::iterator _it = find_if(ties.begin(), ties.end(), hasPoint);
			while (_it != ties.end())
			{
				ties.erase(_it);
				_it = find_if(ties.begin(), ties.end(), hasPoint);
			}
			return;
		}
		void erase(const Tie& tie) {
			vector<Tie>::iterator it = find(ties.begin(), ties.end(), tie);
			if (it == ties.end()) return;
			ties.erase(it);
			return;
		}

		void tick(float dt /*时间变化量*/) {
			for (auto& tie : ties) {
				elasticity(tie);
			}
			for (auto& obj : objs) {
				obj->update(dt);
				obj->F = vec3(0, 0, 0);
			}
			t += dt;
			return;
		}

	};

}
