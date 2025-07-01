#pragma once

#include "Core/Window.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"

#include "Graphics/Renderer.h"
#include "Graphics/RendererLayer.h"

#include "KuchCraft/GameLayer.h"

namespace KuchCraft {

	class Application
	{
	public:
		Application(int argc, char** argv);

		~Application();

		void Run();

		void Close();

		void Restart();

		static inline Application& Get() { return *s_Instance; }

		Config& GetConfig() { return m_Config; }

		Ref<Window> GetWindow() { return m_Window; }

		LayerStack& GetLayerStack() { return m_LayerStack; }

		Ref<Renderer> GetRenderer() { return m_Renderer; }

		bool ShouldRestart() const { return m_Restart; }

	private:
		void OnApplicationEvent(ApplicationEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnShutdown();
		void ProcessEvents();

		void InitImGui();
		bool ShouldBlockEventFromImGui(ApplicationEventType type) const;

	private:
		bool m_Running   = true;
		bool m_Minimized = false;
		bool m_Restart   = false;

		Config                m_Config;
		std::filesystem::path m_ConfigPath = "config.json";

		Ref<Window> m_Window;
		LayerStack  m_LayerStack;

		Timestep m_TickTimer = 0.0f;

		Ref<Renderer> m_Renderer;

		Ref<GameLayer>     m_GameLayer;
		Ref<RendererLayer> m_RendererLayer;

	private:
		static inline Application* s_Instance = nullptr;
	};

}