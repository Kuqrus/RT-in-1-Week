#pragma once

#include "Utils.h"
#include "Material.h"
#include "Ray.h"
#include "Interval.h"

class Material;

struct HitRecord
{
	glm::vec3 p{ 0.0f };
	glm::vec3 Normal{ 0.0f };
	Material* material = nullptr;
	float t = 0.0f;
	bool frontFace = true;

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

class HittableList : public Hittable {
public:
	std::vector<Hittable*> m_Objects;

	HittableList() {}

	//HittableList(std::shared_ptr<Hittable> obj) {
	//	m_Objects.push_back(obj);
	//}
	//void Add(std::shared_ptr<Hittable>obj) {
	//	m_Objects.push_back(obj);
	//}

	HittableList(Hittable* obj) {
		m_Objects.push_back(obj);
	}
	void Add(Hittable* obj) {
		m_Objects.push_back(obj);
	}
	void Clear() {
		m_Objects.clear();
	}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override {
		HitRecord tempRec;
		bool isHit = false;
		float closest = rayT.max;
		for (const auto& obj : m_Objects) {
			if (obj->Hit(ray, Interval(rayT.min, closest), tempRec)) {
				isHit = true;
				closest = tempRec.t;
				rec = tempRec;
			}
		}

		return isHit;
	}
};