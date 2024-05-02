#pragma once
#include <Walnut/Image.h>
#include <execution>
#include <Walnut/Random.h>

#include "Utils.h"
#include "Ray.h"
#include "Camera.h"
#include "Hittable.h"
#include "Sphere.h"
#include "Material.h"
#include "Interval.h"

class Renderer {
public:
	struct Settings
	{
		bool isAntiAliasing = false;
		bool isAccumulate = true;
		bool isOtpimisedRandom = false;
		bool isPotatoMode = true;

		float Gamma = 0.5f;
		float DefocusAngle = 0.0f;
		float FocusDist = 10.0f;
	};
public:
	Renderer() = default;

	void OnResize(uint32_t w, uint32_t h);
	void Render(const Camera& camera, const HittableList& world);

	glm::vec4 PerPixel(uint32_t x, uint32_t y);

	float HitSphere(const glm::vec3& center, float radius, const Ray& ray);
	glm::vec3 RayColor(const Ray& r, int depth);

	Ray GetRay(int x, int y) const;
	glm::vec3 SampleSquare() const;

	glm::vec3 DefocusDiskSample() const;

	std::shared_ptr<Walnut::Image> GetFinalImage()const { return m_FinalImage; };
	Settings& GetSetings() { return m_Settings; }


private:
	const Camera* m_ActiveCamera = nullptr;
	const HittableList* m_World = nullptr;
	Settings m_Settings;

	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;

	glm::vec4* m_AccumilationData = nullptr;
	uint32_t m_FrameIndex = 1;

	std::vector<uint32_t> m_ImgHorizIter, m_ImgVertIter;

	// Antialias params
	int m_SamplesPerPixel = 100;
	float m_PixelSamplesScale = 0.0f;
	//glm::vec3 m_pixel00_loc{0.0f};
	//glm::vec3 m_pixel_delta_u{0.0f};
	//glm::vec3 m_pixel_delta_v{0.0f};

	int m_Depth = 100;
	uint32_t m_Seed = 0;

	glm::vec3 m_DefocusDiskU;
	glm::vec3 m_DefocusDiskV;
};
