#pragma once

#include "Core/Layer.h"
#include "Graphics/Renderer.h"

namespace KuchCraft {

	class GameLayer : public Layer
	{
	public:
		GameLayer(const Ref<Renderer>& renderer);
		virtual ~GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnTick(const Timestep ts) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;
		virtual void OnApplicationEvent(ApplicationEvent& e) override;

	private:
		Ref<Renderer>  m_Renderer;
		Ref<Texture2D> m_GridTexture;

		float m_Rotation = glm::radians(0.0f);
		float m_ColorR   = 0.0f;
	};

}