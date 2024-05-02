#pragma once

//#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <cstdlib>
#include <Walnut/Random.h>

// can replace with FLT_MAX
const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385f;


namespace Utils {
	// can replace with glm::radians(float degrees)
	inline float DegreesToRadians(float degrees) {
		return degrees * pi / 180.0f;
	}

	//bool NearZero(glm::vec3& v) {
	//	float s = 1e-8f;
	//	return (std::fabs(v[0]) < s) && (std::fabs(v[1]) < s) && (std::fabs(v[2]) < s);
	//}

	/*
	inline float RandomFloat() { return rand() / (RAND_MAX + 1.0); }
	inline float RandomFloat(float min, float max) { return min + (max - min) * RandomFloat(); }
	/*
	static glm::vec3 RandomVec3() { return glm::vec3(RandomFloat()); }
	static glm::vec3 RandomVec3(float min, float max) { return glm::vec3(RandomFloat(min, max)); }
	inline glm::vec3 RandimVec3InUnitSphere() {
		while (true) {
			glm::vec3 p = RandomVec3(-1.0f, 1.0f); 
			if (glm::dot(p,p) < 1.0f) {	//or dot
				return p;
			}
		}
	}
	inline glm::vec3 RandomVec3Normal() {
		return glm::normalize(RandimVec3InUnitSphere());
	}
	inline glm::vec3 RandomVec3OnHemisphere(const glm::vec3& normal) {
		glm::vec3 onUnitSphere = RandomVec3Normal();
		if (glm::dot(onUnitSphere, normal) > 0.0f) {
			return onUnitSphere;
		}
		else {
			return -onUnitSphere;
		}
	}
	*/
}
