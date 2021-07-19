#include"hittable.h"
#include"tool.h"
#include"vector.h"

bool Sphere::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const
{
	Vector3 oc = r.origin() - center_;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius_ * radius_;
	float discriminant = b * b - a * c;
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center_) / radius_;
			rec.mat = mat_ptr_;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center_) / radius_;
			rec.mat = mat_ptr_;
			return true;
		}
	}
	return false;
}

HittableList::HittableList(Hittable **l, int n)
{
	list_ = l; list_size_ = n;
}

bool HittableList::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	for (int i = 0; i < list_size_; i++) {
		if (list_[i]->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}