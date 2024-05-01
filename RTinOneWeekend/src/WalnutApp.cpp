#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include "HittableList.h"



class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : m_Camera(45.0f, 0.1f, 100.0f)
	{
		//HittableList* world = new HittableList(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f);
		Sphere* s1 = new Sphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f);
		Sphere* s2 = new Sphere(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f);
		m_World.Add(s1);
		m_World.Add(s2);
	}

	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Checkbox("Enable Anti Aliasing", &m_Renderer.GetSetings().isAntiAliasing);
		ImGui::Checkbox("Enable Accumulation", &m_Renderer.GetSetings().isAccumulate);
		ImGui::Checkbox("Enable Optimised Random", &m_Renderer.GetSetings().isOtpimisedRandom);
		ImGui::DragFloat("Gamma", &m_Renderer.GetSetings().Gamma, 0.01f, 0.0f, 1.0f);
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

		if(!m_Renderer.GetSetings().isAntiAliasing)
			Render();

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