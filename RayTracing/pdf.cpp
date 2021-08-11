#include"pdf.h"

void ONB::build_from_w(const Vector3& n)
{
	axis_[2] = unit_vector(n);
	Vector3 a;
	if (fabs(axis_[2].x()) > 0.9)
		a = Vector3(0, 1, 0);
	else
		a = Vector3(1, 0, 0);
	axis_[1] = unit_vector(cross(axis_[2], a));
	axis_[0] = cross(axis_[2], axis_[1]);
}

CosinePDF::CosinePDF(const CosinePDF& a) 
{
	this->uvw_.axis_[0] = a.uvw_.axis_[0]; 
	this->uvw_.axis_[1] = a.uvw_.axis_[1];
	this->uvw_.axis_[2] = a.uvw_.axis_[2];
}

float CosinePDF::value(const Vector3& direction) const
{
	float cosine = dot(unit_vector(direction), uvw_.axis_[2]);
	if (cosine > 0) return cosine / M_PI;
	else return 0;
}

Vector3 CosinePDF::generate() const
{
	return uvw_.local(random_cosine_direction());
}

float HittablePDF::value(const Vector3& direction) const 
{
	return ptr_->pdf_value(o_, direction);
}

Vector3 HittablePDF::generate() const
{
	return ptr_->random(o_);
}

float MixturePDF::value(const Vector3& direction) const 
{
	return 0.5 * p_[0]->value(direction) + 0.5 *p_[1]->value(direction);
}
Vector3 MixturePDF::generate() const 
{
	if (random_double() < 0.5)
		return p_[0]->generate();
	else
		return p_[1]->generate();
}