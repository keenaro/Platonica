#pragma once
#include <vector>
#include <glm\vec3.hpp>

class PerlinNoise {
public:
	PerlinNoise(unsigned int seed = 0);
	float PNoise(const glm::vec3& pos, const glm::ivec3& periodicPos);
	void Reseed(unsigned int seed);

private:
	float Fade(float t);
	float Lerp(float t, float a, float b);
	float Grad(int hash, float x, float y, float z);

	std::vector<int> permutation;
};

