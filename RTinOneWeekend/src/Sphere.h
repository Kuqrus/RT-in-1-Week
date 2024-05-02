#pragma once

#include "Utils.h"

class Sphere : public Hittable {
public:
	Sphere(const glm::vec3& center, float radius, Material* material) 
		: m_Center(center), m_Radius((float)fmax(0.0, radius)), m_Material(material) {}

	bool Hit(const Ray& ray, Interval rayT, HitRecord& rec) const override {
		glm::vec3 oc = m_Center - ray.Origin();
		float a = glm::length(ray.Direction());
		float h = glm::dot(ray.Direction(), oc);
		float c = glm::dot(oc, oc) - m_Radius * m_Radius;
		float discriminant = h * h - a * c;

		if (discriminant < 0) {
			return false;
		}

		float sqrtDisc = glm::sqrt(discriminant);

		float root = (h - sqrtDisc) / a;
		if (!rayT.Surrounds(root)) {
			root = (h + sqrtDisc) / a;
			if (!rayT.Surrounds(root))
				return false;
		}

		rec.material = m_Material;
		rec.t = root;
		rec.p = ray.At(rec.t);
		glm::vec3 outNormal = (rec.p - m_Center) / m_Radius;
		rec.SetFaceNormal(ray, outNormal);

		return true;
	}

private:
	glm::vec3 m_Center;
	float m_Radius;
	Material* m_Material = nullptr;
};
