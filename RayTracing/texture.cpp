#include "texture.h"

Vector3 CheckerTexture::value(float u, float v, const Vector3& p) const 
{
	float sines = sin(10 * p.x()) *sin(10 * p.y())*sin(10 * p.z());
	if (sines < 0) return odd_->value(u, v, p);
	else return even_->value(u, v, p);
}