#include "camera.h"
#include"tool.h"
Camera::Camera(Vector3 lookfrom, Vector3 lookat, Vector3 vup, float vfov, float aspect,float aperture,float focus_dist)
{
	lens_radius_ = aperture / 2;
	float theta = vfov * M_PI / 180;
	float half_height = tan(theta / 2);
	float half_width = aspect * half_height;
	origin_ = lookfrom;
	w_ = unit_vector(lookfrom - lookat);
	u_ = unit_vector(cross(vup, w_));
	v_ = cross(w_, u_);
	lower_left_corner_ = origin_ - focus_dist*(half_width * u_ + half_height * v_ + w_);
	horizontal_ = 2 * focus_dist * half_width*u_;
	vertical_ = 2 * focus_dist * half_height*v_;
}
Ray Camera::get_ray(float s, float t) {
	Vector3 rd = lens_radius_ * random_in_unit_disk();
	Vector3 offset = u_ * rd.x() + v_ * rd.y();
	return Ray(origin_+offset,lower_left_corner_ +s * horizontal_ + t * vertical_ - origin_-offset);
}