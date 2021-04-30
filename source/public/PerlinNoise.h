#pragma once
#include <vector>
#include <glm\vec3.hpp>

class PerlinNoise {
public:
	static float PNoise(const glm::vec3& pos, const glm::ivec3& periodicPos);
	static float PNoise(const glm::vec3& pos);
	static void Reseed(unsigned int seed);

private:
	static float Fade(float t);
	static float Lerp(float t, float a, float b);
	static float Grad(int hash, float x, float y, float z);

	static std::vector<int> permutation;

	static inline int32_t fastfloor(float fp) {
		int32_t i = static_cast<int32_t>(fp);
		return (fp < i) ? (i - 1) : (i);
	}
};

