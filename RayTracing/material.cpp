#include"vector.h"
#include"material.h"
#include"tool.h"

Vector3 reflect(const Vector3& v, const Vector3&n)
{
	return v - 2 * dot(v, n)*n;
}

bool refract(const Vector3 & v, const Vector3 &n, float ni_over_nt, Vector3& refracted) 
{
	Vector3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt*(1 - dt * dt);
	if (discriminant > 0) 
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else 
		return false;
}

float schlick(float cosine, float ref_idx) 
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0)*pow((1 - cosine), 5);
}


Vector3 Material::emitted(const Ray& r_in, const hit_record& rec, float u, float v, const Vector3& p) const
{
	return Vector3(0, 0, 0);
}

float Lambertian::scattering_pdf(const Ray& r_in, const hit_record& rec, const Ray& scattered) const
{
	float cosine = dot(rec.normal, unit_vector(scattered.direction()));
	if (cosine < 0)
		return 0;
	return cosine / M_PI;
}

bool Lambertian::scatter(const Ray& r_in, const hit_record &rec, Vector3& attenuation, Ray& scattered, float& pdf) const
{
	ONB uvw;
	uvw.build_from_w(rec.normal);
	Vector3 direction = uvw.local(random_cosine_direction());
	scattered = Ray(rec.p, unit_vector(direction), r_in.time());
	attenuation = albedo_->value(rec.u, rec.v, rec.p);
	pdf = dot(uvw.axis_[2], scattered.direction()) / M_PI;
	return true;
}

Metal::Metal(const Vector3& a, float f) :albedo_(a) 
{
	if (f < 1) fuzz_ = f;
	else fuzz_ = 1;
}

bool Metal::scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray& scattered) const 
{
	Vector3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
	scattered = Ray(rec.p, reflected+fuzz_*random_in_unit_sphere(),0);
	attenuation = albedo_;
	return (dot(scattered.direction(), rec.normal) > 0);
}

bool Dielectric::scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray&scattered) const 
{
	Vector3 outward_normal;
	Vector3 reflected = reflect(r_in.direction(), rec.normal);
	float ni_over_nt = ref_idx_;
	attenuation = Vector3(1.0, 1.0, 1.0);
	Vector3 refracted;

	float  reflect_prob;
	float cosine;
	if (dot(r_in.direction(), rec.normal) > 0) 
	{
		outward_normal = -rec.normal;
		ni_over_nt = ref_idx_;
		cosine = ref_idx_ * dot(r_in.direction(), rec.normal)/ r_in.direction().length();
	}
	else 
	{
		outward_normal = rec.normal;
		ni_over_nt = 1.0 / ref_idx_;
		cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
	}

	if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) 
	{
		reflect_prob = schlick(cosine, ref_idx_);
	}
	else {
		reflect_prob = 1.0;
	}

	if (random_double() < reflect_prob) {
		scattered = Ray(rec.p, reflected,0);
	}
	else {
		scattered = Ray(rec.p, refracted,0);
	}

	return true;
}

bool DiffuseLight::scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray& scattered) const
{
	return false;
}

Vector3  DiffuseLight::emitted(const Ray& r_in, const hit_record& rec, float u, float v, const Vector3& p) const
{
	if (dot(rec.normal, r_in.direction()) < 0.0)
		return emit_->value(u, v, p);
	else
		return Vector3(0, 0, 0);
}

bool Isotropic::scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray& scattered) const
{
	scattered = Ray(rec.p, random_in_unit_sphere(),0);
	attenuation = albedo_->value(rec.u, rec.v, rec.p);
	return true;
}
