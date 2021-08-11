#include"hittable.h"
#include"tool.h"
#include"vector.h"

inline Vector3 random_to_sphere(float radius, float distance_squared) {
	float r1 = random_double();
	float r2 = random_double();
	float z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	float phi = 2 * M_PI*r1;
	float x = cos(phi)*sqrt(1 - z * z);
	float y = sin(phi)*sqrt(1 - z * z);
	return Vector3(x, y, z);
}

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

float Sphere::pdf_value(const Vector3& o, const Vector3& v) const {
	hit_record rec;
	if (this->hit(Ray(o, v,0), 0.001, FLT_MAX, rec)) {
		float cos_theta_max = sqrt(1 - radius_ * radius_ / (center_ - o).squared_length());
		float solid_angle = 2 * M_PI*(1 - cos_theta_max);
		return  1 / solid_angle;
	}
	else
		return 0;
}

Vector3 Sphere::random(const Vector3& o) const 
{
	Vector3 direction = center_ - o;
	float distance_squared = direction.squared_length();
	ONB uvw;
	uvw.build_from_w(direction);
	return uvw.local(random_to_sphere(radius_, distance_squared));
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

float HittableList::pdf_value(const Vector3& o, const Vector3& v) const {
	float weight = 1.0 / list_size_;
	float sum = 0;
	for (int i = 0; i < list_size_; i++)
		sum += weight * list_[i]->pdf_value(o, v);
	return sum;
}

Vector3 HittableList::random(const Vector3& o) const {
	int index = int(random_double() * list_size_);
	return list_[index]->random(o);
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

bool XYRect::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const
{
	float t = (k_ - r.origin().z()) / r.direction().z();
	if (t<t_min || t>t_max) return false;
	float x = r.origin().x() + t * r.direction().x();
	float y = r.origin().y() + t * r.direction().y();
	if (x < x0_ || x > x1_ || y < y0_ || y > y1_) return false;
	rec.u = (x - x0_) / (x1_ - x0_);
	rec.v = (y-  y0_) / (y1_ -  y0_);
	rec.t = t;
	rec.mat = mat_;
	rec.p = r.point_at_parameter(t);
	rec.normal = Vector3(0, 0, 1);
	return true;
}

bool XZRect::hit(const Ray& r, float t0, float t1, hit_record& rec) const {
	float t = (k_ - r.origin().y()) / r.direction().y();
	if (t < t0 || t > t1)
		return false;
	float x = r.origin().x() + t * r.direction().x();
	float z = r.origin().z() + t * r.direction().z();
	if (x < x0_ || x > x1_ || z < z0_ || z > z1_)
		return false;
	rec.u = (x - x0_) / (x1_ - x0_);
	rec.v = (z - z0_) / (z1_ - z0_);
	rec.t = t;
	rec.mat = mat_;
	rec.p = r.point_at_parameter(t);
	rec.normal = Vector3(0, 1, 0);
	return true;
}

float XZRect::pdf_value(const Vector3& o, const Vector3& v) const 
{
	hit_record rec;
	if (this->hit(Ray(o, v,0), 0.001, FLT_MAX, rec)) {
		float area = (x1_ - x0_)*(z1_ - z0_);
		float distance_squared = rec.t * rec.t * v.squared_length();
		float cosine = fabs(dot(v, rec.normal) / v.length());
		return  distance_squared / (cosine * area);
	}
	else
		return 0;
}

Vector3 XZRect::random(const Vector3& o) const {
	Vector3 random_point = Vector3(x0_ + random_double()*(x1_ - x0_), k_,
		z0_ + random_double()*(z1_ - z0_));
	return random_point - o;
}

bool YZRect::hit(const Ray& r, float t0, float t1, hit_record& rec) const {
	float t = (k_ - r.origin().x()) / r.direction().x();
	if (t < t0 || t > t1)
		return false;
	float y = r.origin().y() + t * r.direction().y();
	float z = r.origin().z() + t * r.direction().z();
	if (y < y0_ || y > y1_ || z < z0_ || z > z1_)
		return false;
	rec.u = (y - y0_) / (y1_ - y0_);
	rec.v = (z - z0_) / (z1_ - z0_);
	rec.t = t;
	rec.mat = mat_;
	rec.p = r.point_at_parameter(t);
	rec.normal = Vector3(1, 0, 0);
	return true;
}

bool FlipNormals::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const 
{
	if (ptr_->hit(r, t_min, t_max, rec)) {
		rec.normal = -rec.normal;
		return true;
	}
	else
		return false;
}

Box::Box(const Vector3& p0, const Vector3& p1, Material *ptr) {
	p_min_ = p0;
	p_max_ = p1;
	Hittable **list = new Hittable*[6];
	list[0] = new XYRect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
	list[1] = new FlipNormals(new XYRect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));
	list[2] = new XZRect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
	list[3] = new FlipNormals(new XZRect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));
	list[4] = new YZRect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
	list[5] = new FlipNormals(new YZRect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
	list_ = new HittableList(list, 6);
}

bool Box::hit(const Ray& r, float t0, float t1, hit_record& rec) const {
	return list_->hit(r, t0, t1, rec);
}

bool Translate::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const 
{
	Ray move_r(r.origin() - offset_, r.direction(), r.time());
	if (ptr_->hit(move_r, t_min, t_max, rec)) 
	{
		rec.p += offset_;
		return true;
	}
	else  return false;
}

bool Translate::bounding_box(float t0, float t1, AABB& box) const 
{
	if (ptr_->bounding_box(t0, t1, box)) 
	{
		box = AABB(box.min() + offset_, box.max() + offset_);
		return true;
	}
	return false;
}

RotateY::RotateY(Hittable *p, float angle) : ptr_(p) {
	float radians = (M_PI / 180.) * angle;
	sin_theta_ = sin(radians);
	cos_theta_ = cos(radians);
	hasbox_ = ptr_->bounding_box(0, 1, bbox_);
	Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				float x = i * bbox_.max().x() + (1 - i)*bbox_.min().x();
				float y = j * bbox_.max().y() + (1 - j)*bbox_.min().y();
				float z = k * bbox_.max().z() + (1 - k)*bbox_.min().z();
				float newx = cos_theta_ * x + sin_theta_ * z;
				float newz = -sin_theta_ * x + cos_theta_ * z;
				Vector3 tester(newx, y, newz);
				for (int c = 0; c < 3; c++)
				{
					if (tester[c] > max[c])
						max[c] = tester[c];
					if (tester[c] < min[c])
						min[c] = tester[c];
				}
			}
		}
	}
	bbox_ = AABB(min, max);
}

bool RotateY::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
	Vector3 origin = r.origin();
	Vector3 direction = r.direction();
	origin[0] = cos_theta_ * r.origin()[0] - sin_theta_ * r.origin()[2];
	origin[2] = sin_theta_ * r.origin()[0] + cos_theta_ * r.origin()[2];
	direction[0] = cos_theta_ * r.direction()[0] - sin_theta_ * r.direction()[2];
	direction[2] = sin_theta_ * r.direction()[0] + cos_theta_ * r.direction()[2];
	Ray rotated_r(origin, direction, r.time());
	if (ptr_->hit(rotated_r, t_min, t_max, rec)) {
		Vector3 p = rec.p;
		Vector3 normal = rec.normal;
		p[0] = cos_theta_ * rec.p[0] + sin_theta_ * rec.p[2];
		p[2] = -sin_theta_ * rec.p[0] + cos_theta_ * rec.p[2];
		normal[0] = cos_theta_ * rec.normal[0] + sin_theta_ * rec.normal[2];
		normal[2] = -sin_theta_ * rec.normal[0] + cos_theta_ * rec.normal[2];
		rec.p = p;
		rec.normal = normal;
		return true;
	}
	else
		return false;
}

bool ConstantMedium::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const 
{
	hit_record rec1, rec2;
	if (boundary_->hit(r, -FLT_MAX, FLT_MAX, rec1)) 
	{
		if (boundary_->hit(r, rec1.t + 0.0001, FLT_MAX, rec2)) 
		{
			if (rec1.t < t_min)
				rec1.t = t_min;

			if (rec2.t > t_max)
				rec2.t = t_max;

			if (rec1.t >= rec2.t)
				return false;

			if (rec1.t < 0)
				rec1.t = 0;

			float distance_inside_boundary = (rec2.t - rec1.t)*r.direction().length();
			float hit_distance = -(1 / density_) * log(random_double());

			if (hit_distance < distance_inside_boundary) {

				rec.t = rec1.t + hit_distance / r.direction().length();
				rec.p = r.point_at_parameter(rec.t);

				rec.normal = Vector3(1, 0, 0);  // arbitrary
				rec.mat = phase_function_;
				return true;
			}
		}
	}
	return false;
}