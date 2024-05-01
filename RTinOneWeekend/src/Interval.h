#pragma once
#include <float.h>

struct Interval
{
	float min, max;
	Interval() : min(+FLT_MAX), max(-FLT_MAX) {}
	Interval(float _min, float _max) : min(_min), max(_max) {}

	float Size() const { return max - min; }
	bool Contains(float x) const { return min <= x && x <= max; }
	bool Surrounds(float x) const { return min < x && x < max; }

	float Clamp(float x) const {
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	//static const Interval Empty, Universe;
};

//const Interval Interval::Empty = Interval(+FLT_MAX, -FLT_MAX);
//const Interval Interval::Universe = Interval(-FLT_MAX, +FLT_MAX);