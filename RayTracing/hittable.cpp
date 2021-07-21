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

bool Sphere::bounding_box(float t0, float t1, AABB& box) const
{
	box = AABB(center_ - Vector3(radius_, radius_, radius_), center_ + Vector3(radius_, radius_, radius_));
	return true;
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

bool HittableList::bounding_box(float t0, float t1, AABB& box) const {
	if (list_size_ < 1) return false;
	AABB temp_box;
	bool first_true = list_[0]->bounding_box(t0, t1, temp_box);
	if (!first_true)
		return false;
	else
		box = temp_box;
	for (int i = 1; i < list_size_; i++) {
		if (list_[i]->bounding_box(t0, t1, temp_box)) {
			box = surrounding_box(box, temp_box);
		}
		else
			return false;
	}
	return true;
}

Vector3 MoveSphere::center(float time) const 
{
	return center0_ + ((time - time0_) / (time1_ - time0_))*(center1_ - center0_);
}

bool MoveSphere::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const 
{
	Vector3 oc = r.origin() - center(r.time());
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
			rec.normal = (rec.p - center(r.time())) / radius_;
			rec.mat = mat_ptr_;
			return true;
		}
		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius_;
			rec.mat = mat_ptr_;
			return true;
		}
	}
	return false;
}

bool MoveSphere::bounding_box(float t0, float t1, AABB& box) const
{
	AABB box0(center(t0) - Vector3(radius_, radius_, radius_),
		center(t0) + Vector3(radius_, radius_, radius_));
	AABB box1(center(t1) - Vector3(radius_, radius_, radius_),
		center(t1) + Vector3(radius_, radius_, radius_));
	box = surrounding_box(box0, box1);
	return true;
}

int box_x_compare(const void * a, const void * b) {
	AABB box_left, box_right;
	Hittable *ah = *(Hittable**)a;
	Hittable *bh = *(Hittable**)b;

	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	if (box_left.min().x() - box_right.min().x() < 0.0)
		return -1;
	else
		return 1;
}

int box_y_compare(const void * a, const void * b) {
	AABB box_left, box_right;
	Hittable *ah = *(Hittable**)a;
	Hittable *bh = *(Hittable**)b;

	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	if (box_left.min().y() - box_right.min().y() < 0.0)
		return -1;
	else
		return 1;
}

int box_z_compare(const void * a, const void * b) {
	AABB box_left, box_right;
	Hittable *ah = *(Hittable**)a;
	Hittable *bh = *(Hittable**)b;

	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";

	if (box_left.min().z() - box_right.min().z() < 0.0)
		return -1;
	else
		return 1;
}

BvhNode::BvhNode(Hittable **l, int n, float time0, float time1) {
	int axis = int(3 * random_double());

	if (axis == 0)
		qsort(l, n, sizeof(Hittable *), box_x_compare);
	else if (axis == 1)
		qsort(l, n, sizeof(Hittable *), box_y_compare);
	else
		qsort(l, n, sizeof(Hittable *), box_z_compare);

	if (n == 1) {
		left_ = right_ = l[0];
	}
	else if (n == 2) {
		left_ = l[0];
		right_ = l[1];
	}
	else {
		left_ = new BvhNode(l, n / 2, time0, time1);
		right_ = new BvhNode(l + n / 2, n - n / 2, time0, time1);
	}

	AABB box_left, box_right;

	if (!left_->bounding_box(time0, time1, box_left) ||
		!right_->bounding_box(time0, time1, box_right)) {

		std::cerr << "no bounding box in bvh_node constructor\n";
	}

	box_ = surrounding_box(box_left, box_right);
}

bool BvhNode::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
	if (box_.hit(r, t_min, t_max)) {
		hit_record left_rec, right_rec;
		bool hit_left = left_->hit(r, t_min, t_max, left_rec);
		bool hit_right = right_->hit(r, t_min, t_max, right_rec);
		if (hit_left && hit_right) {
			if (left_rec.t < right_rec.t)
				rec = left_rec;
			else
				rec = right_rec;
			return true;
		}
		else if (hit_left) {
			rec = left_rec;
			return true;
		}
		else if (hit_right) {
			rec = right_rec;
			return true;
		}
		else
			return false;
	}
	else return false;
}

bool BvhNode::bounding_box(float t0, float t1, AABB& b) const {
	b = box_;
	return true;
}


