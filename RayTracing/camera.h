#pragma once
#ifndef CAMERAH
#define CAMERAH
#include"ray.h"

#define M_PI 3.14159265358
class Camera 
{
public:
	Camera(Vector3 lookfrom, Vector3 lookat, Vector3 vup, float vfov, float aspect, float aperture, float focus_dist,float time0,float time1);
	Ray get_ray(float u, float v);
	Vector3 origin_;
	Vector3 lower_left_corner_;
	Vector3 horizontal_;
	Vector3 vertical_;
	Vector3 u_, v_, w_;
	float lens_radius_;
	float time0_, time1_;
};

#endif // !CAMERAH
