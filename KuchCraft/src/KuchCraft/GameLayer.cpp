#include "kcpch.h"
#include "KuchCraft/GameLayer.h"

namespace KuchCraft {

	GameLayer::GameLayer(const Ref<Renderer>& renderer)
		: Layer("GameLayer", LayerType::Game), m_Renderer(renderer)
	{
		m_GridTexture = Texture2D::Create(std::filesystem::path("assets/textures/grid.png"));

		m_Scene = CreateRef<Scene>("Example Scene");
		m_Scene->SetRenderer(renderer);

		auto camera = m_Scene->CreateEntity("Camera");
		camera.AddComponent<CameraComponent>();
		m_Scene->SetPrimaryCamera(camera);

		auto quad = m_Scene->CreateEntity("Quad");
		auto& sprite = quad.AddComponent<SpriteRendererComponent>();
		sprite.Color = { 1.0f, 0.0f, 0.0f, 1.0f };
		auto& transform = quad.GetTransform();
		transform.Translation = { 500.0f, 200.0f, 0.0f };
		transform.Scale = { 175.0f, 200.0f, 1.0f };
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

		m_Scene->OnUpdate(ts);
	}

	void GameLayer::OnTick(const Timestep ts)
	{
		m_Scene->OnTick(ts);
	}

	void GameLayer::OnRender()
	{
		m_Renderer->DrawQuad2D({ 300.0f, 600.0f }, { 200.0f, 100.0f }, m_GridTexture);

		m_Renderer->DrawRotatedQuad2D({ 100.0f, 400.0f }, { 100.0f, 100.0f }, glm::radians(25.0f), { 0.0f, 0.0f, 1.0f, 1.0f });
		m_Renderer->DrawRotatedQuad2D({ 300.0f, 400.0f }, { 100.0f, 100.0f }, glm::radians(50.0f), { 0.0f, 1.0f, 0.0f, 1.0f });
		m_Renderer->DrawRotatedQuad2D({ 500.0f, 400.0f }, { 100.0f, 100.0f }, glm::radians(75.0f), { 1.0f, 0.0f, 0.0f, 1.0f });

		m_Renderer->DrawRotatedQuad2D({ 700.0f,  350.0f }, { 150.0f, 150.0f }, -m_Rotation, { m_ColorR, 0.0f, 1.0f, 1.0f });

		m_Scene->OnRender();
	}

	void GameLayer::OnImGuiRender()
	{
	}

	void GameLayer::OnApplicationEvent(ApplicationEvent& e)
	{
		m_Scene->OnApplicationEvent(e);
	}
}
