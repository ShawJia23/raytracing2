#pragma once
#ifndef HITTABLEH
#define HITTABLEH

#include"ray.h"
#include"vector.h"

class Material;

struct hit_record
{
	float t;
	Vector3 p;
	Vector3 normal;
	Material *mat;
};

class Hittable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

class HittableList : public Hittable {
public:
	HittableList() {}
	HittableList(Hittable **l, int n);
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	Hittable **list_;
	int list_size_;
};

class Sphere :public Hittable
{
public:
	Sphere() {}
	Sphere(Vector3 center, float r,Material* m) :center_(center), radius_(r),mat_ptr_(m) {};

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec)const;

	Vector3 center_;
	float radius_;
	Material *mat_ptr_;
};

#endif 