#include"aabb.h"


bool AABB::hit(const  Ray& r, float t_min, float t_max)const 
{
	for (int a = 0; a < 3; a++)
	{
		float invD = 1.0f / r.direction()[a];
		//float t0 = ffmin((min_[a] - r.origin()[a]) / r.direction()[a], (max_[a] - r.origin()[a]) / r.direction()[a]);
		//float t1 = ffmax((min_[a] - r.origin()[a]) / r.direction()[a], (max_[a] - r.origin()[a]) / r.direction()[a]);
		float t0 = (min()[a] - r.origin()[a]) * invD;
		float t1 = (max()[a] - r.origin()[a]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);
		t_min = t0 > t_min ? t0 : t_min;
		t_max = t1 < t_max ? t1 : t_max;
		if (t_max <= t_min) return false;
	}
	return true;
}

AABB surrounding_box(AABB box0, AABB box1) {
	Vector3 small(ffmin(box0.min().x(), box1.min().x()),
		ffmin(box0.min().y(), box1.min().y()),
		ffmin(box0.min().z(), box1.min().z()));
	Vector3 big(ffmax(box0.max().x(), box1.max().x()),
		ffmax(box0.max().y(), box1.max().y()),
		ffmax(box0.max().z(), box1.max().z()));
	return AABB(small, big);
}