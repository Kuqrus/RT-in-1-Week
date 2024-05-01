#include "Renderer.h"

namespace Utils {
	float linearToGamma(float f) {
		if (f > 0) {
			return glm::sqrt(f);
		}
		return 0;
	}

	uint32_t ConvertToRGBA(const glm::vec4& color) {
		//uint8_t r = color.r * 255.0f;
		//uint8_t g = color.g * 255.0f;
		//uint8_t b = color.b * 255.0f;
		//uint8_t a = color.a * 255.0f;

		//static const Interval intensity(0.0f, 0.999f);
		//uint8_t r = intensity.Clamp(color.r) * 255.0f;
		//uint8_t g = intensity.Clamp(color.g) * 255.0f;
		//uint8_t b = intensity.Clamp(color.b) * 255.0f;

		uint8_t r = static_cast<uint8_t>(glm::clamp(linearToGamma(color.r), 0.0f, 0.999f) * 255.0f);
		uint8_t g = static_cast<uint8_t>(glm::clamp(linearToGamma(color.g), 0.0f, 0.999f) * 255.0f);
		uint8_t b = static_cast<uint8_t>(glm::clamp(linearToGamma(color.b), 0.0f, 0.999f) * 255.0f);

		uint8_t a = static_cast<uint8_t>(color.a * 255.0f);

		uint32_t res = (a << 24) | (b << 16) | (g << 8) | r;
		return res;
	}
	//uint32_t ConvertToRGB(const glm::vec3& color) {
	//	static const Interval intensity(0.0f, 0.999f);
	//	uint8_t r = intensity.Clamp(color.r) * 255.0f;
	//	uint8_t g = intensity.Clamp(color.g) * 255.0f;
	//	uint8_t b = intensity.Clamp(color.b) * 255.0f;
	//	uint8_t a = 1.0f * 255.0f;
	//
	//	uint32_t res = (a << 24) | (b << 16) | (g << 8) | r;
	//	return res;
	//}

	static uint32_t PCG_Hash(uint32_t input) {
		uint32_t state = input * 747796405u + 2891336453u;
		//uint32_t word = ((state >> (state >> 28u) + 4u) ^ state) * 277803737u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}
	static float RandomFloat(uint32_t& seed) {
		seed = PCG_Hash(seed);
		return (float)seed / (float)UINT32_MAX;
	}
	static glm::vec3 InUnitSphere(uint32_t& seed) {
		return glm::normalize(glm::vec3(RandomFloat(seed) * 2.0f - 1.0f,
										RandomFloat(seed) * 2.0f - 1.0f,
										RandomFloat(seed) * 2.0f - 1.0f));
	}

	inline glm::vec3 RandomInUnitSphere() {
		while (true) {
			glm::vec3 p = Walnut::Random::Vec3(-1.0f, 1.0f);
			if (glm::dot(p, p) < 1.0f) {
				return p;
			}
		}
	}
	static glm::vec3 RandomOnHemisphere(const glm::vec3& normal) {
		glm::vec3 onUnitSphere = glm::normalize(Utils::RandomInUnitSphere());
		if (glm::dot(onUnitSphere, normal) > 0.0f) {
			return onUnitSphere;
		}
		else {
			return -onUnitSphere;
		}
	}
}

void Renderer::OnResize(uint32_t w, uint32_t h)
{
	if (m_FinalImage) {
		if (m_FinalImage->GetWidth() == w && m_FinalImage->GetHeight() == h) return;
		m_FinalImage->Resize(w, h);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(w, h, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[w * h];

	delete[] m_AccumilationData;
	m_AccumilationData = new glm::vec4[w * h];

	m_ImgHorizIter.resize(w);
	m_ImgVertIter.resize(h);

	for (uint32_t i = 0; i < w; i++) {
		m_ImgHorizIter[i] = i;
	}
	for (uint32_t i = 0; i < h; i++) {
		m_ImgVertIter[i] = i;
	}
}

void Renderer::Render(const Camera& camera, const HittableList& world)
{
	m_ActiveCamera = &camera;
	m_World = &world;
	m_PixelSamplesScale = 1.0f / m_SamplesPerPixel;

	//float aspectRatio = m_FinalImage->GetWidth() / m_FinalImage->GetHeight();

	//float imgHeight = m_FinalImage->GetHeight();
	float viewHeight = 2.0f;

	//float imgWight = m_FinalImage->GetWidth();
	float viewWight = viewHeight * ((float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight());

	float focalLenght = 1.0f;

	glm::vec3 cameraCenter(0.0f);	

	glm::vec3 viewportU(viewWight, 0.0f, 0.0f);
	glm::vec3 viewportV(0.0f, -viewHeight, 0.0f);

	auto pixel_delta_u = viewportU / (float)m_FinalImage->GetWidth();
	auto pixel_delta_v = viewportV / (float)m_FinalImage->GetHeight();

	m_pixel_delta_u = pixel_delta_u;
	m_pixel_delta_v = pixel_delta_v;

	auto viewport_upper_left = cameraCenter
		- glm::vec3(0.0f, 0.0f, focalLenght) - viewportU / 2.0f - viewportV / 2.0f;
	auto pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);
	m_pixel00_loc = pixel00_loc;

	if (m_FrameIndex == 1) {
		memset(m_AccumilationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
	}

// render
	if (m_Settings.isAntiAliasing) {
		std::for_each(std::execution::par, m_ImgVertIter.begin(), m_ImgVertIter.end(),
			[&](uint32_t y) {
				std::for_each(std::execution::par, m_ImgHorizIter.begin(), m_ImgHorizIter.end(),
				[&](uint32_t x)
					{
						glm::vec3 pixel_color(0.0f);
						uint32_t seed = x + y * m_FinalImage->GetWidth();
						for (int i = 0; i < m_SamplesPerPixel; i++) {
							Ray r = GetRay(x, y);							
							pixel_color += RayColor(r, m_Depth, seed);
						}

						m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(glm::vec4(pixel_color * m_PixelSamplesScale, 1.0f));

						//m_AccumilationData[x + y * m_FinalImage->GetWidth()] += glm::vec4(pixel_color, 1.0f);
						//glm::vec4 accumulatedColor = m_AccumilationData[x + y * m_FinalImage->GetWidth()];
						//accumulatedColor /= m_FrameIndex;
						//accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
						//m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
					});
			});
	}
	else {
		std::for_each(std::execution::par, m_ImgVertIter.begin(), m_ImgVertIter.end(),
			[&](uint32_t y) {
				std::for_each(std::execution::par, m_ImgHorizIter.begin(), m_ImgHorizIter.end(),
				[&](uint32_t x)
					{
						/*	Book method, static camera	*/
						//auto pixel_center = pixel00_loc + ((float)x * pixel_delta_u) + ((float)y * pixel_delta_v);
						//auto ray_direction = pixel_center - cameraCenter; 

						/*	Cherno method, movable camera	*/
						//glm::vec3 pixel_center = pixel00_loc + ((float)x * pixel_delta_u) + ((float)y * pixel_delta_v);

						glm::vec3 ray_origin = m_ActiveCamera->GetPosition();
						glm::vec3 ray_direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

						Ray r(ray_origin, ray_direction);
						uint32_t seed = x + y * m_FinalImage->GetWidth();
						glm::vec3 pixel_color = RayColor(r, m_Depth, seed);
						//m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGB(pixel_color);

						m_AccumilationData[x+y*m_FinalImage->GetWidth()] += glm::vec4(pixel_color, 1.0f);
						glm::vec4 accumulatedColor = m_AccumilationData[x + y * m_FinalImage->GetWidth()];
						accumulatedColor /= m_FrameIndex;
						accumulatedColor = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
						m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);

					});
			});
	}
	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.isAccumulate) {
		m_FrameIndex++;
	}
	else {
		m_FrameIndex = 1;
	}
}

// mb delete later
glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	float r = float(x) / (m_FinalImage->GetWidth() - 1);
	float g = float(y) / (m_FinalImage->GetHeight() - 1);
	float b = 0.0f;
	float a = 1.0f;

	return glm::vec4(r, g, b, a);
}

// delete later
float Renderer::HitSphere(const glm::vec3& center, float radius, const Ray& ray)
{
	glm::vec3 oc = center - ray.Origin();

	//float a = glm::dot(ray.Direction(), ray.Direction());
	float a = glm::length(ray.Direction());

	float h = glm::dot(ray.Direction(), oc);

	//float b = -2.0f * glm::dot(ray.Direction(), oc);

	float c = glm::dot(oc, oc) - radius * radius;
	//float c = glm::length(oc) - radius * radius;

	//float discriminant = b * b - 4 * a * c;
	float discriminant = h * h - a * c;

	if (discriminant < 0) {
		return -1.0f;
	}
	else {
		return (h - sqrt(discriminant)) / a;
		//return (-b - sqrt(discriminant) ) / (2.0*a);
	}
}

glm::vec3 Renderer::RayColor(const Ray& r, int depth, uint32_t seed)
{
	//float t = HitSphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, r);
	//if (t > 0.0f) {
	//	glm::vec3 N = glm::normalize(r.At(t) - glm::vec3(0.0f, 0.0f, -1.0f));
	//	glm::vec4 color((0.5f * glm::vec3(N.x + 1.0f, N.y + 1.0f, N.z + 1.0f)), 1.0f);
	//	return color;
	//}

	if (depth <= 0) {
		return glm::vec3(0.0f);
	}

	HitRecord rec;

	if (m_World->Hit(r, Interval(0.0001f, FLT_MAX), rec)) {
		glm::vec3 direction;
		if (m_Settings.isOtpimisedRandom) {
			seed *= m_FrameIndex;
			seed += depth;
			glm::vec3 rand = Utils::InUnitSphere(seed);
			direction = glm::dot(rand, rec.Normal) > 0 ? rand : -rand;
		}
		else {
			direction = Utils::RandomOnHemisphere(rec.Normal);
		}
		direction += rec.Normal;
		return m_Settings.Gamma * RayColor(Ray(rec.p, direction), depth - 1, seed);
	}

	float a = 0.5f * (glm::normalize(r.Direction()).y + 1.0f);
	glm::vec3 skyColor = (1.0f - a) * glm::vec3(1.0f) + a * glm::vec3(0.5f, 0.7f, 1.0f);
	return skyColor;	
}


// both func need for antialiasing, but it works incorrectly
Ray Renderer::GetRay(int x, int y) const
{
	glm::vec3 offset = SampleSquare();
	//glm::vec3 pixelSample = m_ActiveCamera->GetPosition()
	//	+ ((x + offset.x) * m_pixel_delta_u)
	//	+ ((y + offset.y) * m_pixel_delta_v);

	x = x + offset.x;
	y = y + offset.y;

	glm::vec3 rayOrigin = m_ActiveCamera->GetPosition();
	glm::vec3 rayDirection = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

	return Ray(rayOrigin, rayDirection);
}
glm::vec3 Renderer::SampleSquare() const
{
	return Walnut::Random::Vec3(-0.5f, 0.5f);
}
