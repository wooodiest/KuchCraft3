#pragma once

#include "Core/Layer.h"
#include "Graphics/Renderer.h"

#include "Scene/Scene.h"

namespace KuchCraft {

	class GameLayer : public Layer
	{
	public:
		GameLayer(const Ref<Renderer>& renderer, Config config);
		virtual ~GameLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnTick(const Timestep ts) override;
		virtual void OnRender() override;
		virtual void OnImGuiRender() override;
		virtual void OnApplicationEvent(ApplicationEvent& e) override;

		void CreateWorld(const std::string& name);
		void LoadWorld  (const std::string& name);
		void DeleteWorld(const std::string& name);

	private:
		void DrawEntityNode(Entity entity);

	private:
		UUID m_HierarchyPanelSelectedEntity = 0;

	private:
		Ref<Renderer> m_Renderer;
		Ref<Scene> m_Scene;
		Config m_Config;
		std::string m_CurrentWorldName;
	};

}