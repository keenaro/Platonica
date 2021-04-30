#include "PerlinNoise.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

std::vector<int> PerlinNoise::permutation;

void PerlinNoise::Reseed(unsigned int seed)
{
	permutation.resize(256);
	std::iota(permutation.begin(), permutation.end(), 0);
	std::default_random_engine engine(seed);
	std::shuffle(permutation.begin(), permutation.end(), engine);
	permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

float PerlinNoise::PNoise(const glm::vec3& pos, const glm::ivec3& periodicPos)
{
	int ix0, iy0, ix1, iy1, iz0, iz1;
	float fx0, fy0, fz0, fx1, fy1, fz1;
	float s, t, r;
	float nxy0, nxy1, nx0, nx1, n0, n1;

	ix0 = fastfloor(pos.x);
	iy0 = fastfloor(pos.y);
	iz0 = fastfloor(pos.z);
	fx0 = pos.x - ix0;
	fy0 = pos.y - iy0;
	fz0 = pos.z - iz0;
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	fz1 = fz0 - 1.0f;
	ix1 = ((ix0 + 1) % periodicPos.x) & 255;
	iy1 = ((iy0 + 1) % periodicPos.y) & 255;
	iz1 = ((iz0 + 1) % periodicPos.z) & 255;
	ix0 = (ix0 % periodicPos.x) & 255;
	iy0 = (iy0 % periodicPos.y) & 255;
	iz0 = (iz0 % periodicPos.z) & 255;

	r = Fade(fz0);
	t = Fade(fy0);
	s = Fade(fx0);

	nxy0 = Grad(permutation[ix0 + permutation[iy0 + permutation[iz0]]], fx0, fy0, fz0);
	nxy1 = Grad(permutation[ix0 + permutation[iy0 + permutation[iz1]]], fx0, fy0, fz1);
	nx0 = Lerp(r, nxy0, nxy1);

	nxy0 = Grad(permutation[ix0 + permutation[iy1 + permutation[iz0]]], fx0, fy1, fz0);
	nxy1 = Grad(permutation[ix0 + permutation[iy1 + permutation[iz1]]], fx0, fy1, fz1);
	nx1 = Lerp(r, nxy0, nxy1);

	n0 = Lerp(t, nx0, nx1);

	nxy0 = Grad(permutation[ix1 + permutation[iy0 + permutation[iz0]]], fx1, fy0, fz0);
	nxy1 = Grad(permutation[ix1 + permutation[iy0 + permutation[iz1]]], fx1, fy0, fz1);
	nx0 = Lerp(r, nxy0, nxy1);

	nxy0 = Grad(permutation[ix1 + permutation[iy1 + permutation[iz0]]], fx1, fy1, fz0);
	nxy1 = Grad(permutation[ix1 + permutation[iy1 + permutation[iz1]]], fx1, fy1, fz1);
	nx1 = Lerp(r, nxy0, nxy1);

	n1 = Lerp(t, nx0, nx1);

	return 0.936f * (Lerp(s, n0, n1));
}

float PerlinNoise::PNoise(const glm::vec3& pos)
{
	int ix0, iy0, ix1, iy1, iz0, iz1;
	float fx0, fy0, fz0, fx1, fy1, fz1;
	float s, t, r;
	float nxy0, nxy1, nx0, nx1, n0, n1;

	ix0 = fastfloor(pos.x);
	iy0 = fastfloor(pos.y);
	iz0 = fastfloor(pos.z);
	fx0 = pos.x - ix0;
	fy0 = pos.y - iy0;
	fz0 = pos.z - iz0;
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	fz1 = fz0 - 1.0f;
	ix0 &= 255;
	iy0 &= 255;
	iz0 &= 255;
	ix1 = (ix0 + 1) & 255;
	iy1 = (iy0 + 1) & 255;
	iz1 = (iz0 + 1) & 255;

	r = Fade(fz0);
	t = Fade(fy0);
	s = Fade(fx0);

	nxy0 = Grad(permutation[ix0 + permutation[iy0 + permutation[iz0]]], fx0, fy0, fz0);
	nxy1 = Grad(permutation[ix0 + permutation[iy0 + permutation[iz1]]], fx0, fy0, fz1);
	nx0 = Lerp(r, nxy0, nxy1);

	nxy0 = Grad(permutation[ix0 + permutation[iy1 + permutation[iz0]]], fx0, fy1, fz0);
	nxy1 = Grad(permutation[ix0 + permutation[iy1 + permutation[iz1]]], fx0, fy1, fz1);
	nx1 = Lerp(r, nxy0, nxy1);

	n0 = Lerp(t, nx0, nx1);

	nxy0 = Grad(permutation[ix1 + permutation[iy0 + permutation[iz0]]], fx1, fy0, fz0);
	nxy1 = Grad(permutation[ix1 + permutation[iy0 + permutation[iz1]]], fx1, fy0, fz1);
	nx0 = Lerp(r, nxy0, nxy1);

	nxy0 = Grad(permutation[ix1 + permutation[iy1 + permutation[iz0]]], fx1, fy1, fz0);
	nxy1 = Grad(permutation[ix1 + permutation[iy1 + permutation[iz1]]], fx1, fy1, fz1);
	nx1 = Lerp(r, nxy0, nxy1);

	n1 = Lerp(t, nx0, nx1);

	return 0.936f * (Lerp(s, n0, n1));
}


float PerlinNoise::Fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::Lerp(float t, float a, float b) {
	return a + t * (b - a);
}

float PerlinNoise::Grad(int hash, float x, float y, float z) {
	const int h = hash & 15;
	const float u = h < 8 ? x : y;
	const float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}