#pragma once
#ifndef PDFH
#define PDFH
#include "vector.h"
#include "tool.h"
#include "hittable.h"

class Hittable;

class ONB
{
public:
	ONB() {}
	Vector3 operator[](int i)const { return axis_[i]; }
	Vector3 local(float x, float y, float z)const { return x * axis_[0] + y * axis_[1] + z * axis_[2]; }
	Vector3 local(const Vector3&a) const { return a.x()* axis_[0] + a.y() * axis_[1] + a.z()* axis_[2]; }
	void build_from_w(const Vector3& n);
	Vector3 axis_[3];//uvw
};

class PDF
{
public:
	virtual float value(const Vector3& direction) const = 0;
	virtual Vector3 generate() const = 0;
};

class CosinePDF :public PDF
{
public:
	CosinePDF(const Vector3& w) { uvw_.build_from_w(w); }
	virtual float value(const Vector3& direction) const;
	virtual Vector3 generate() const;
	ONB uvw_;
};

class HittablePDF : public PDF {
public:
	HittablePDF(Hittable *p, const Vector3& origin) : ptr_(p), o_(origin) {}
	virtual float value(const Vector3& direction) const;
	virtual Vector3 generate() const;
	Vector3 o_;
	Hittable *ptr_;
};

class MixturePDF : public PDF {
public:
	MixturePDF(PDF *p0, PDF *p1) { p_[0] = p0; p_[1] = p1; }
	virtual float value(const Vector3& direction) const;
	virtual Vector3 generate() const;
	PDF *p_[2];
};

#endif // !PDFH