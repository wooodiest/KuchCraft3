#include "kcpch.h"
#include "KuchCraft/GameLayer.h"

namespace KuchCraft {

	GameLayer::GameLayer(const Ref<Renderer>& renderer)
		: Layer("GameLayer", LayerType::Game), m_Renderer(renderer)
	{
	}

	GameLayer::~GameLayer()
	{
	}

	void GameLayer::OnAttach()
	{
	}

	void GameLayer::OnDetach()
	{
	}

	void GameLayer::OnUpdate(Timestep ts)
	{
		m_Rotation += glm::radians(30.0f) * ts;
		m_ColorR = glm::abs(glm::sin(m_Rotation));		
	}

	void GameLayer::OnTick(const Timestep ts)
	{
	}

	void GameLayer::OnRender()
	{
		m_Renderer->DrawQuad2D({ 800.0f, 100.0f }, { 100.0f, 100.0f }, { 1.0f, 0.0f, 1.0f, 1.0f });

		m_Renderer->DrawQuad2D({ 100.0f, 100.0f }, { 100.0f, 100.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
		m_Renderer->DrawQuad2D({ 300.0f, 100.0f }, { 100.0f, 100.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });
		m_Renderer->DrawQuad2D({ 500.0f, 100.0f }, { 100.0f, 100.0f }, { 0.0f, 0.0f, 1.0f, 1.0f });

		m_Renderer->DrawRotatedQuad2D({ 100.0f, 400.0f }, { 100.0f, 100.0f }, glm::radians(25.0f), { 0.0f, 0.0f, 1.0f, 1.0f });
		m_Renderer->DrawRotatedQuad2D({ 300.0f, 400.0f }, { 100.0f, 100.0f }, glm::radians(50.0f), { 0.0f, 1.0f, 0.0f, 1.0f });
		m_Renderer->DrawRotatedQuad2D({ 500.0f, 400.0f }, { 100.0f, 100.0f }, glm::radians(75.0f), { 1.0f, 0.0f, 0.0f, 1.0f });

		m_Renderer->DrawRotatedQuad2D({ 1100.0f, 350.0f }, { 150.0f, 150.0f },  m_Rotation, { 0.0f,     0.0f, 1.0f, 1.0f });
		m_Renderer->DrawRotatedQuad2D({ 700.0f,  350.0f }, { 150.0f, 150.0f }, -m_Rotation, { m_ColorR, 0.0f, 1.0f, 1.0f });
	}

	void GameLayer::OnImGuiRender()
	{
	}

	void GameLayer::OnApplicationEvent(ApplicationEvent& e)
	{
	}
}
