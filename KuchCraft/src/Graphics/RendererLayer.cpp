#include "kcpch.h"
#include "Graphics/RendererLayer.h"

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
	}

	void RendererLayer::OnApplicationEvent(ApplicationEvent& e)
	{
	}
}