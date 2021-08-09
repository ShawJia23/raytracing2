#include<float.h>
#include<random>
#include <functional>
#include"vector.h"
#include"tool.h"

inline double random_double()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);//连续均匀分布
	static std::mt19937 generator;//随机数产生器 产生随的机数的周期长，可达到2^19937-1。
	static std::function<double()> reand_generator = std::bind(distribution, generator);
	return reand_generator();
}

Vector3 random_in_unit_sphere() {
	Vector3 p;
	do {
		p = 2.0*Vector3(random_double(), random_double(), random_double()) - Vector3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}

Vector3 random_in_unit_disk() {
	Vector3 p;

	do {
		p = 2.0*Vector3(random_double(), random_double(), 0) - Vector3(1, 1, 0);
	} while (dot(p, p) >= 1.0);
	return p;
}

Vector3 random_on_unit_sphere() {
	Vector3 p;
	do {
		p = 2.0*Vector3(random_double(), random_double(), random_double()) - Vector3(1, 1, 1);
	} while (dot(p, p) >= 1.0);
	return unit_vector(p);
}

Vector3 random_cosine_direction() 
{
	float r1 = random_double();
	float r2 = random_double();
	float z = sqrt(1 - r2);
	float phi = 2 * M_PI*r1;
	float x = cos(phi)*sqrt(r2);
	float y = sin(phi)*sqrt(r2);
	return Vector3(x, y, z);
}

void ONB::build_from_w(const Vector3& n) 
{
	axis_[2] = unit_vector(n);
	Vector3 a;
	if (fabs(axis_[2].x()) > 0.9)
		a = Vector3(0, 1, 0);
	else
		a = Vector3(1, 0, 0);
	axis_[1] = unit_vector(cross(axis_[2], a));
	axis_[0] = cross(axis_[2], axis_[1]);
}