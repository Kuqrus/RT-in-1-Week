#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Camera.h"
#include "Hittable.h"
#include "Material.h"

class ExampleLayer : public Walnut::Layer
{
public:
	/*
	ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f)
	{		
		{	// Sphere ground
			glm::vec3 position(0.0f, -100.5f, -1.0f);
			float radius = 100.0f;
			glm::vec3 materialColor(0.8f, 0.8f, 0.0f);
			Lambertian* material = new Lambertian(materialColor);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
		{	// Sphere center
			glm::vec3 position(0.0f, 0.0f, -1.2f);
			float radius = 0.5f;
			glm::vec3 materialColor(0.1f, 0.2f, 0.5f);
			Lambertian* material = new Lambertian(materialColor);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}		
		{	// Sphere left
			glm::vec3 position(-1.0f, 0.0f, -1.0f);
			float radius = 0.5f;
			Dielectric* material = new Dielectric(1.5f);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
		{	// Sphere left (bubble)
			glm::vec3 position(-1.0f, 0.0f, -1.0f);
			float radius = 0.4f;
			Dielectric* material = new Dielectric(1.0f / 1.5f);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
		{	// Sphere right
			glm::vec3 position(1.0f, 0.0f, -1.0f);
			float radius = 0.5f;
			glm::vec3 materialColor(0.8f, 0.6f, 0.2f);
			Metal* material = new Metal(materialColor, 1.0f);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
	}*/

	ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f)
	{
		{
			glm::vec3 position(0.0f, -1000.0f, 0.0f);
			glm::vec3 albedo(0.5f, 0.5f, 0.5f);
			Lambertian* material = new Lambertian(albedo);
			Sphere* sphere = new Sphere(position, 1000.0f, material);
			m_World.Add(sphere);
		}
		{
			for (int a = -11; a < 11; a++) {
				for (int b = -11; b < 11; b++)
				{
					float chooseMat = Walnut::Random::Float();
					glm::vec3 center(a + 0.9 * Walnut::Random::Float(), 0.2f, b + 0.9 * Walnut::Random::Float());
					if ((center - glm::vec3(4.0f, 0.2f, 0.0f)).length > 0) {
						if (chooseMat < 0.8) {
							glm::vec3 albedo = Walnut::Random::Vec3() * Walnut::Random::Vec3();
							Lambertian* material = new Lambertian(albedo);
							Sphere* sphere = new Sphere(center, 0.2f, material);
							m_World.Add(sphere);
						}
						else if (chooseMat < 0.95) {
							glm::vec3 albedo = Walnut::Random::Vec3(0.5f, 1.0f);
							float fuzz = Walnut::Random::Vec3(0.0f, 0.5f).x;
							Metal* material = new Metal(albedo, fuzz);
							Sphere* sphere = new Sphere(center, 0.2f, material);
							m_World.Add(sphere);
						}
						else {
							Dielectric* material = new Dielectric(1.5f);
							Sphere* sphere = new Sphere(center, 0.2f, material);
							m_World.Add(sphere);
						}
					}
				}
			}
		}
		{
			glm::vec3 position(0.0f, 1.0f, 0.0f);
			float radius = 1.0f;
			Dielectric* material = new Dielectric(1.5f);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
		{
			glm::vec3 position(-4.0f, 1.0f, 0.0f);
			float radius = 1.0f;
			glm::vec3 albedo(0.4f, 0.2f, 0.1f);
			Lambertian* material = new Lambertian(albedo);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
		{
			glm::vec3 position(4.0f, 1.0f, 0.0f);
			float radius = 1.0f;
			glm::vec3 albedo(0.7f, 0.6f, 0.5f);
			float fuzz = 0.0f;
			Metal* material = new Metal(albedo, fuzz);
			Sphere* sphere = new Sphere(position, radius, material);
			m_World.Add(sphere);
		}
	}


	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Checkbox("Enable Potato Mode", &m_Renderer.GetSetings().isPotatoMode);
		ImGui::Checkbox("Enable Anti Aliasing", &m_Renderer.GetSetings().isAntiAliasing);
		ImGui::Checkbox("Enable Accumulation", &m_Renderer.GetSetings().isAccumulate);
		ImGui::Checkbox("Enable Optimised Random", &m_Renderer.GetSetings().isOtpimisedRandom);
		ImGui::DragFloat("Gamma", &m_Renderer.GetSetings().Gamma, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("DefocusAngle", &m_Renderer.GetSetings().DefocusAngle, 0.01f, 0.0f, 100.0f);
		ImGui::DragFloat("FocusDist", &m_Renderer.GetSetings().FocusDist, 0.01f, 0.0f, 100.0f);
		if (ImGui::Button("Restore focus"))
		{
			m_Renderer.GetSetings().DefocusAngle = 0.0;
			m_Renderer.GetSetings().FocusDist = 10.0f;
		}
		ImGui::DragFloat3("Camera pos", glm::value_ptr(m_Camera.m_Position));
		ImGui::DragFloat3("Camera dir", glm::value_ptr(m_Camera.m_ForwardDirection));
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
			Render();
		ImGui::End();



		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidht = (uint32_t)ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = (uint32_t)ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();

		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::PopStyleVar();
		ImGui::End();

		//if(!m_Renderer.GetSetings().isAntiAliasing)
			//Render();

	}
	void Render() {
		Walnut::Timer timer;

		m_Renderer.OnResize(m_ViewportWidht, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidht, m_ViewportHeight);
		m_Renderer.Render(m_Camera, m_World);

		m_LastRenderTime = timer.ElapsedMillis();

	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	HittableList m_World;
	uint32_t m_ViewportWidht = 0;
	uint32_t m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}