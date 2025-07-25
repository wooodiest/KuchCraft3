#pragma once

#include "Core/Layer.h"
#include "Graphics/Renderer.h"

#include "Scene/Scene.h"

namespace KuchCraft {

	enum class GameState
	{
		MainMenu,
		InGame,
		PauseMenu
	};

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

		GameState GetGameState() const { return m_GameState; }

		void CreateWorld(const std::string& name);
		void LoadWorld  (const std::string& name);
		void DeleteWorld(const std::string& name);

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		void ImGui_DrawEntityNode(Entity entity);
		void ImGui_DrawMainMenuUI();
		void ImGui_DrawGameUI();
		void ImGui_DrawPauseMenuUI();

	private:
		UUID m_HierarchyPanelSelectedEntity = 0;

		ItemID m_PerviousItemID = -1;
		ItemID m_SelectedItemID = 0;
		Ref<Texture2D> m_SelectedItemTexture;

	private:
		Ref<Renderer> m_Renderer;
		Ref<Scene>    m_Scene;
		Config        m_Config;
		GameState     m_GameState = GameState::MainMenu;
		std::string   m_CurrentWorldName;
	};

}