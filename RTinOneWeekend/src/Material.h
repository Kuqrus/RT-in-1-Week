#pragma once

#include "Utils.h"
#include "Hittable.h"
#include "Ray.h"

class Ray;
struct HitRecord;

class Material {
public:
	virtual ~Material() = default;

	virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const;
};

class Lambertian : public Material {
public:
	Lambertian(glm::vec3& albedo) : m_Albedo(albedo) {}

	bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override;


private:
	glm::vec3 m_Albedo;
};

class Metal : public Material {
public:
	Metal(glm::vec3& albedo, float fuzz)
		: m_Albedo(albedo), m_Fuzz(fuzz < 1.0f ? fuzz : 1.0f) {}

	bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override;

private:
	glm::vec3 m_Albedo;
	float m_Fuzz;
};

class Dielectric : public Material {
public:
	Dielectric(float refractionIndex) : m_RefractionIndex(refractionIndex) {}

	bool Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override;

private:
	float m_RefractionIndex;

	static float Reflectance(float cosine, float refractionIndex);
};


