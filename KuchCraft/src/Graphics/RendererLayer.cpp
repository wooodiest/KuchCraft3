#include "kcpch.h"
#include "Graphics/RendererLayer.h"

#include <imgui.h>

namespace KuchCraft {

	RendererLayer::RendererLayer(const Ref<Renderer>& renderer)
		: Layer("RendererLayer", LayerType::Debug), m_Renderer(renderer)
	{
		m_GridTexture = Texture2D::Create(std::filesystem::path("assets/textures/grid.png"));
	}

	RendererLayer::~RendererLayer()
	{
	}

	void RendererLayer::OnAttach()
	{
	}

	void RendererLayer::OnDetach()
	{
	}

	void RendererLayer::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(KeyCode::F10))
		{
			if (m_State == LayerState::Active)
				SetState(LayerState::NotVisible);
			else if (m_State == LayerState::NotVisible)
				SetState(LayerState::Active);
		}
	}

	void RendererLayer::OnTick(const Timestep ts)
	{
	}

	void RendererLayer::OnRender()
	{
		m_Renderer->DrawQuad2D({ 850.0f, 150.0f }, { 100.0f, 100.0f }, { 0.0f, 1.0f, 1.0f, 1.0f });
		m_Renderer->DrawQuad2D({ 300.0f, 700.0f }, { 300.0f, 300.0f }, { 1.0f, 1.0f, 0.0f, 1.0f });
		m_Renderer->DrawQuad2D({ 900.0f, 500.0f }, { 100.0f, 100.0f }, m_GridTexture);
	}

	void RendererLayer::OnImGuiRender()
	{
		ImGui::Begin("Render Debug Tools");

		if (ImGui::CollapsingHeader("Statistics##Renderer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const auto& stats = m_Renderer->GetStats();

			ImGui::Text("Draw calls: %d", stats.DrawCalls);
			ImGui::Text("Vertices: %d", stats.Vertices);
			ImGui::Text("Quads: %d", stats.Quads);
		}

		ImGui::End();
	}

	void RendererLayer::OnApplicationEvent(ApplicationEvent& e)
	{

	}
}