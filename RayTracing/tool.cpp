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