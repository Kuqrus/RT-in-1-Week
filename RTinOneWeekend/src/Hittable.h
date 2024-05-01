#pragma once

#include "Ray.h"

struct HitRecord
{
	glm::vec3 p;
	glm::vec3 Normal;
	float t;
	bool frontFace;

	void SetFaceNormal(const Ray& ray, const glm::vec3& outNormal) {
		frontFace = glm::dot(ray.Direction(), outNormal) < 0;
		if (frontFace)
			Normal = outNormal;
		else
			Normal = -outNormal;
	}

};

class Hittable {
public:
	virtual ~Hittable() = default;
	virtual bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const = 0;
};
