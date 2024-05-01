#pragma once
#include "Utils.h"

class Ray {
public:
	Ray() {}
	Ray(const glm::vec3& origin, const glm::vec3& direction) : orig(origin), dir(direction) {}

	const glm::vec3& Origin() const { return orig; }
	const glm::vec3& Direction() const { return dir; }

	glm::vec3 At(float t) const {
		return orig + t * dir;
	}

private:
	glm::vec3 orig{ 0.0f, 0.0f, 0.0f };
	glm::vec3 dir{ 0.0f, 0.0f, 0.0f };
};