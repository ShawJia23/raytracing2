#pragma once
#ifndef TEXTUREH
#define TEXTUREH
#include "vector.h"
#include "perlin.h"
#include <math.h>
#include"tool.h"

class Texture 
{
public:
	virtual Vector3 value(float u, float v, const Vector3& p)const = 0;
};

class ConstantTexture :public Texture 
{
public:
	ConstantTexture() {}
	ConstantTexture(Vector3 c):color_(c) {}
	virtual Vector3 value(float u, float v, const Vector3& p) const { return color_; }
	Vector3 color_;
};

class CheckerTexture :public Texture 
{
public:
	CheckerTexture() {}
	CheckerTexture(Texture *t0,Texture *t1):even_(t0),odd_(t1) {}
	virtual Vector3 value(float u, float v, const Vector3& p) const;
	Texture* odd_;
	Texture* even_;
};

class NoiseTexture : public Texture {
public:
	NoiseTexture() {}
	virtual Vector3 value(float u, float v, const Vector3& p) const {
		return Vector3(1, 1, 1) * 0.5 * (1 + sin(scale_ * p.z() + 10 * noise_.turb(p)));
	}
	Perlin noise_;
	float scale_;
};

void get_sphere_uv(const Vector3& p, float & u, float & v);

class ImageTexture :public Texture 
{
public:
	ImageTexture() {}
	ImageTexture(unsigned char *pixels, int a, int b) :data_(pixels),nx_(a),ny_(b){}
	virtual Vector3 value(float u, float v, const Vector3& p)const;
	unsigned char *data_;
	int nx_, ny_;
};
#endif // !TEXTUREH

