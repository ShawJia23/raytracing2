#include "perlin.h"

float perlin_Interp(Vector3 c[2][2][2], float u, float v, float w)
{
	float uu = u * u * (3 - 2 * u);
	float vv = v * v * (3 - 2 * v);
	float ww = w * w * (3 - 2 * w);
	float accum = 0;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
			{
				Vector3 weightV(u - i, v - j, w - k);
				accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weightV);
			}
	return accum;
}

static Vector3* perlin_generate()
{
	Vector3* p = new Vector3[256];
	for (int i = 0; i < 256; i++) {
		p[i] = unit_vector(Vector3(2 * random_double() - 1, 2 * random_double() - 1, 2 * random_double() - 1));
	}
	return p;
}

void permute(int *p, int n) {
	for (int i = n - 1; i > 0; i--) {
		int target = int(random_double()*(i + 1));
		int tmp = p[i];
		p[i] = p[target];
		p[target] = tmp;
	}
	return;
}

static int* perlin_generate_perm() {
	int * p = new int[256];
	for (int i = 0; i < 256; i++)
		p[i] = i;
	permute(p, 256);
	return p;
}

float PerlinNoise::noise(const Vector3& p) const {
	float u = p.x() - floor(p.x());
	float v = p.y() - floor(p.y());
	float w = p.z() - floor(p.z());

	int i = floor(p.x());
	int j = floor(p.y());
	int k = floor(p.z());

	Vector3 c[2][2][2];
	for (int di = 0; di < 2; di++)
		for (int dj = 0; dj < 2; dj++)
			for (int dk = 0; dk < 2; dk++)
				c[di][dj][dk] = random_vector3_[perm_x_[(i + di) & 255] ^ perm_x_[(j + dj) & 255] ^ perm_x_[(k + dk) & 255]];
	return perlin_Interp(c, u, v, w);
}

float PerlinNoise::turb(const Vector3& p, int depth)const
{
	float accum = 0.0f;
	Vector3 temp = p;
	float weight = 1.0f;
	for (int i = 0; i < depth; i++)
	{
		accum += weight * noise(temp);
		weight *= 0.5f;
		temp *= 2;
	}
	return fabs(accum);
}

Vector3 *PerlinNoise::random_vector3_ = perlin_generate();
int *PerlinNoise::perm_x_ = perlin_generate_perm();
int *PerlinNoise::perm_y_ = perlin_generate_perm();
int *PerlinNoise::perm_z_ = perlin_generate_perm();