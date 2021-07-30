#pragma once
#ifndef HITTABLEH
#define HITTABLEH

#include"ray.h"
#include"vector.h"
#include"AABB.h"

class Material;

struct hit_record
{
	float t;//参数t
	Vector3 p;//相交点
	Vector3 normal;
	Material *mat;
};

int box_x_compare(const void * a, const void * b);

int box_y_compare(const void * a, const void * b);

int box_z_compare(const void * a, const void * b);

class Hittable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(float t0, float t1, AABB& box) const = 0;
};

class HittableList : public Hittable {
public:
	HittableList() {}
	HittableList(Hittable **l, int n);
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	Hittable **list_;
	int list_size_;
};

class Sphere :public Hittable
{
public:
	Sphere() {}
	Sphere(Vector3 center, float r,Material* m) :center_(center), radius_(r),mat_ptr_(m) {};

	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	Vector3 center_;
	float radius_;
	Material *mat_ptr_;
};

class MoveSphere :public Hittable 
{
public:
	MoveSphere() {}
	MoveSphere(Vector3 center0, Vector3 center1, float time0, float time1, float r, Material *m) :
		center0_(center0), center1_(center1), time0_(time0), time1_(time1), radius_(r), mat_ptr_(m) {};
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	Vector3 center(float time) const;
	Vector3 center0_, center1_;
	float time0_, time1_;
	float radius_;
	Material *mat_ptr_;
};

class BvhNode :public Hittable 
{
public:
	BvhNode() {}
	BvhNode(Hittable **l, int n, float time0, float time1);
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;

	Hittable *left_;
	Hittable *right_;
	AABB box_;
};

#endif 