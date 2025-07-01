#pragma once

#include "Core/Layer.h"
#include "Graphics/Renderer.h"

namespace KuchCraft {

	class RendererLayer : public Layer
	{
	public:
		RendererLayer(const Ref<Renderer>& renderer);
		virtual ~RendererLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnTick(const Timestep ts) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;
		virtual void OnApplicationEvent(ApplicationEvent& e) override;

	private:
		Ref<Renderer> m_Renderer;
		Ref<Texture2D> m_GridTexture;
	};

}