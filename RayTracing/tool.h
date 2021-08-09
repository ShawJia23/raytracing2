#pragma once
#ifndef TOOLH
#define TOOLH
#include<float.h>
#include<random>
#include <functional>
#include"vector.h"

#define M_PI 3.14159265358

inline double random_double();
Vector3 random_in_unit_sphere();
Vector3 random_in_unit_disk();
Vector3 random_on_unit_sphere();
Vector3 random_cosine_direction();

class ONB 
{
public:
	ONB() {}
	Vector3 operator[](int i)const { return axis_[i]; }
	Vector3 local(float x, float y, float z)const { return x * axis_[0] + y * axis_[1] + z * axis_[2]; }
	Vector3 local(const Vector3&a ) const{ return a.x()* axis_[0] + a.y() * axis_[1] + a.z()* axis_[2]; }
	void build_from_w(const Vector3& n);
	Vector3 axis_[3];//uvw
};

#endif // !TOOLH