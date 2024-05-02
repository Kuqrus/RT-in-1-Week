#include "Material.h"

namespace Utils {
	bool NearZero(glm::vec3& v) 
	{
		float s = 1e-8;
		return (std::fabs(v[0]) < s) && (std::fabs(v[1]) < s) && (std::fabs(v[2]) < s);
	}
	// Optimised random funcs
	static uint32_t PCG_Hash(uint32_t input) 
	{
		uint32_t state = input * 747796405u + 2891336453u;
		//uint32_t word = ((state >> (state >> 28u) + 4u) ^ state) * 277803737u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}
	static float RandomFloat(uint32_t& seed) 
	{
		seed = PCG_Hash(seed);
		return (float)seed / (float)UINT32_MAX;
	}
	static glm::vec3 InUnitSphere(uint32_t& seed) 
	{
		return glm::normalize(glm::vec3(RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f,
			RandomFloat(seed) * 2.0f - 1.0f));
	}

	// Non-optimised random funcs
	inline glm::vec3 RandomInUnitSphere() 
	{
		while (true) {
			glm::vec3 p = Walnut::Random::Vec3(-1.0f, 1.0f);
			if (glm::dot(p, p) < 1.0f) {
				return p;
			}
		}
	}
	static glm::vec3 RandomOnHemisphere(const glm::vec3& normal)
	{
		glm::vec3 onUnitSphere = glm::normalize(Utils::RandomInUnitSphere());
		if (glm::dot(onUnitSphere, normal) > 0.0f) {
			return onUnitSphere;
		}
		else {
			return -onUnitSphere;
		}
	}
	
	// delete later
	inline glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat)
	{
		float cos_theta = fmin(dot(-uv, n), 1.0f);
		glm::vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
		glm::vec3 r_out_parallel = -sqrt(fabs(1.0f - glm::dot(r_out_perp, r_out_perp))) * n;
		return r_out_perp + r_out_parallel;
	}
}


bool Material::Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation,  Ray& scattered) const
{
	return false;
}

bool Lambertian::Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation,  Ray& scattered) const
{
	/*
	glm::vec3 scatterDirection = rec.Normal;
	if(isOptimised)
		scatterDirection += Utils::InUnitSphere(seed);
	else
		scatterDirection += Walnut::Random::Vec3();
	*/

//	glm::vec3 scatterDirection = rec.Normal + Utils::InUnitSphere(seed);	// optimised random
	glm::vec3 scatterDirection = rec.Normal + Walnut::Random::Vec3();		// non optimised random, kinda works same

	if (Utils::NearZero(scatterDirection)) {
		scatterDirection = rec.Normal;
	}

	scattered = Ray(rec.p, scatterDirection);
	attenuation = m_Albedo;
	return true;
}

bool Metal::Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
	glm::vec3 reflected = glm::reflect(rayIn.Direction(), rec.Normal);

	/*
	if(isOptimised)
		reflected = glm::normalize(reflected) + (m_Fuzz * Utils::InUnitSphere(seed));
	else
		reflected = glm::normalize(reflected) + (m_Fuzz * Walnut::Random::Vec3());
	*/

//	reflected = glm::normalize(reflected) + (m_Fuzz * Utils::InUnitSphere(seed));	// optimised random
	reflected = glm::normalize(reflected) + (m_Fuzz * Walnut::Random::Vec3());		// non optimised random, kinda works same

	scattered = Ray(rec.p, reflected);
	attenuation = m_Albedo;
//	return true;											// <- looks a bit cleaner
	return glm::dot(scattered.Direction(), rec.Normal) > 0; // <- has cool shadow on edges, sometimes looks kinda messy
}

bool Dielectric::Scatter(const Ray& rayIn, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
	attenuation = glm::vec3(1.0f);
	float ri = rec.frontFace ? (1.0f / m_RefractionIndex) : m_RefractionIndex;

	glm::vec3 unitDirection = glm::normalize(rayIn.Direction());
	float cosTheta = fmin(glm::dot(-unitDirection, rec.Normal), 1.0f);
	float sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);

	bool cannotRefract = ri * sinTheta > 1.0f;
	glm::vec3 direction;

	if (cannotRefract || Reflectance(cosTheta, ri) > Walnut::Random::Float())
		direction = glm::reflect(unitDirection, rec.Normal);
	else
//		direction = Utils::Refract(unitDirection, rec.Normal, ri);
		direction = glm::refract(unitDirection, rec.Normal, ri);

	scattered = Ray(rec.p, direction);
	return true;
}

float Dielectric::Reflectance(float cosine, float refractionIndex)
{
	float r0 = (1 - refractionIndex) / (1 + refractionIndex);
	r0 = r0 * r0;
	return r0 + (1 - r0) * glm::pow((1 - cosine), 5);
}
