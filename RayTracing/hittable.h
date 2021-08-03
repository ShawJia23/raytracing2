#pragma once
#ifndef HITTABLEH
#define HITTABLEH

#include"ray.h"
#include"vector.h"
#include"AABB.h"

class Material;

struct hit_record
{
	float u;
	float v;
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

class XYRect :public Hittable 
{
public:
	XYRect() {}
	XYRect(float x0, float x1, float y0, float y1, float k, Material *mat):
	x0_(x0),x1_(x1), y0_(y0), y1_(y1),k_(k),mat_(mat) {}
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const 
	{
		box = AABB(Vector3(x0_, y0_, k_ - 0.0001), Vector3(x1_, y1_, k_ + 0.0001));
		return true;
	}
	Material *mat_;
	float x0_, x1_, y0_, y1_,k_;
};

class XZRect :public Hittable
{
public:
	XZRect() {}
	XZRect(float x0, float x1, float z0, float z1, float k, Material *mat) :
		x0_(x0), x1_(x1), z0_(z0), z1_(z1), k_(k), mat_(mat) {}
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const
	{
		box = AABB(Vector3(x0_, k_ - 0.0001,z0_), Vector3(x1_, k_ + 0.0001,z1_));
		return true;
	}
	Material *mat_;
	float x0_, x1_, z0_, z1_, k_;
};

class YZRect :public Hittable
{
public:
	YZRect() {}
	YZRect(float y0, float y1, float z0, float z1, float k, Material *mat) :
		y0_(y0), y1_(y1), z0_(z0), z1_(z1), k_(k), mat_(mat) {}
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const
	{
		box = AABB(Vector3( k_ - 0.0001,y0_,z0_), Vector3(k_ + 0.0001,y1_,z1_));
		return true;
	}
	Material *mat_;
	float y0_, y1_,z0_,z1_ ,k_;
};

class FlipNormals :public Hittable 
{
public:
	FlipNormals(Hittable * p) :ptr_(p) {}
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1,AABB& box) const 
	{
		return ptr_->bounding_box(t0, t1, box);
	}
	Hittable *ptr_;
};

class Box :public Hittable 
{
public:
	Box() {}
	Box(const Vector3& p0, const Vector3& p1, Material *ptr);
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const 
	{
		box = AABB(p_min_, p_max_);
		return true;
	}
	Vector3 p_min_, p_max_;
	Hittable *list_;
};

class Translate :public Hittable 
{
public:
	Translate(Hittable *p, const Vector3& displacement) :ptr_(p), offset_(displacement) {}
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const;
	Hittable *ptr_;
	Vector3 offset_;
};

class RotateY :public Hittable 
{
public:
	RotateY(Hittable *p, float angle);
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const;
	virtual bool bounding_box(float t0, float t1, AABB& box) const 
	{
		box = bbox_; return hasbox_;
	}
	Hittable *ptr_;
	float sin_theta_;
	float cos_theta_;
	bool hasbox_;
	AABB bbox_;
};
#endif 