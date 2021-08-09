#include "texture.h"

Vector3 CheckerTexture::value(float u, float v, const Vector3& p) const 
{
	float sines = sin(10 * p.x()) *sin(10 * p.y())*sin(10 * p.z());
	if (sines < 0) return odd_->value(u, v, p);
	else return even_->value(u, v, p);
}

void get_sphere_uv(const Vector3& p, float & u, float & v)
{
	float phi = atan2(p.z(), p.x());
	float theta = asin(p.y());
	u = 1 - (phi + M_PI) / (2 * M_PI);
	v = (theta + M_PI / 2) / M_PI;
}

Vector3 ImageTexture::value(float u, float v, const Vector3& p) const 
{
	int i = (u)*nx_;
	int j = (1 - v)*ny_ - 0.001;
	if (i < 0)i = 0;
	if (j < 0)j = 0;
	if (i>nx_-1) i = nx_ - 1;
	if (j > ny_ - 1) j = ny_ - 1;
	float r = int(data_[3 * i + 3 * nx_*j]) / 255.0;
	float g = int(data_[3 * i + 3 * nx_*j+1]) / 255.0;
	float b= int(data_[3 * i + 3 * nx_*j+2]) / 255.0;
	return Vector3(r, g, b);
}

//Vector3 NoiseTexture::value(float u, float v, const Vector3& p) const
//{
//	return Vector3(1, 1, 1) * 0.5 * (1 + sin(scale_ * p.z() + 10 * noise_.turb(p)));
//}