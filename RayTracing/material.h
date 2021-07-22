#pragma once
#ifndef MATERIALH
#define MATERIALH
#include"hittable.h"
#include"ray.h"
#include"vector.h"
#include "texture.h"


struct hit_record;

class Material 
{
public:
	virtual bool scatter(const Ray& r_in, const hit_record &rec, Vector3& attenuation, Ray& scattered) const = 0;
};

Vector3 reflect(const Vector3&a, const Vector3&n);

bool refract(const Vector3 & v, const Vector3 &n, float ni_over_nt, Vector3& refracted);

float schlick(float cosine, float ref_idx);

class Lambertian :public Material 
{
public:
	Lambertian(Texture *a) :albedo_(a) {}
	virtual bool scatter(const Ray& r_in, const hit_record &rec, Vector3& attenuation, Ray& scattered) const;
	Texture* albedo_;
};

class Metal : public Material 
{
public:
	Metal(const Vector3& a,float f);
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray& scattered) const;
	Vector3 albedo_;
	float fuzz_;
};

class Dielectric :public Material 
{
public:
	Dielectric(float ri) : ref_idx_(ri) {}
	virtual bool scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray&scattered) const;
	float ref_idx_;

};
#endif // !MATERIALH
