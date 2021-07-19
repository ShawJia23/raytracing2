#pragma once
#ifndef RAYH
#define RAYH
#include "vector.h"

class Ray 
{
public:
	Ray() {}
	Ray(const Vector3& a, const Vector3& b) { A_ = a; B_ = b; }
	Vector3 origin() const { return A_; }
	Vector3 direction() const { return B_; }
	Vector3 point_at_parameter(float t) const { return A_ + t * B_; }

	Vector3 A_;
	Vector3 B_;
};
#endif // !RAYH
