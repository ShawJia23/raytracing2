#ifndef AABBH
#define AABBH
#include"vector.h"
#include "ray.h"

inline float ffmin(float a, float b) { return a < b ? a : b; }

inline float ffmax(float a, float b) { return a > b ? a : b; }

class AABB 
{
public:
	AABB() {}
	AABB(const Vector3& a, const Vector3& b) { min_ = a; max_ = b; }
	Vector3 min() const { return min_; }
	Vector3 max() const { return max_; }

	bool hit(const  Ray& r, float t_min, float t_max)const;
	Vector3 min_;
	Vector3 max_;
};

AABB surrounding_box(AABB box0, AABB box1);

#endif // !AABBH
