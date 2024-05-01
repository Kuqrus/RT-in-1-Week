#pragma once

#include "Hittable.h"

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