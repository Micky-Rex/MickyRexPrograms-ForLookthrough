#pragma once
#include "Basic.h"

const double infinity = numeric_limits<double>::infinity();
class interval
{
public:
	double min, max;
	interval() : min(infinity), max(-infinity) {};
	interval(double min, double max) : min(min), max(max) {};
	double size() const { return max - min; }
	bool contains(double x) const { return min <= x and x <= max; }
	bool surrounds(double x) const { return min < x and x < max; }
	double clamp(double x)const { return std::max(std::min(max, x), min); }
	interval expand(double delta) const { return interval(min - delta * 0.5, max + delta * 0.5); }
	bool operator == (const interval& itv)
	{
		return min == itv.min and max == itv.max;
	}
	bool operator != (const interval& itv)
	{
		return !(*this == itv);
	}
	static const interval empty, universe;
};
const interval interval::empty = interval(infinity, -infinity);
const interval interval::universe = interval(-infinity, infinity);