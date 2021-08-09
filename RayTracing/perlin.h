#pragma once
#ifndef PERLINH
#define PERLINH
#include "vector.h"
#include "tool.h"

class PerlinNoise {
public:
	float noise(const Vector3& p) const;
	float turb(const Vector3& p, int depth = 7)const;
	static Vector3 *random_vector3_;
	static int *perm_x_;
	static int *perm_y_;
	static int *perm_z_;
};

#endif // !PERLINH
