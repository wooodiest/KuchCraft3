#pragma once

#include "Core/Window.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"

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

		bool ShouldRestart() const { return m_Restart; }

	private:
		void OnApplicationEvent(ApplicationEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnShutdown();
		void ProcessEvents();

		void InitImGui();

	private:
		bool m_Running   = true;
		bool m_Minimized = false;
		bool m_Restart   = false;

		Config                m_Config;
		std::filesystem::path m_ConfigPath = "config.json";

		Ref<Window> m_Window;
		LayerStack  m_LayerStack;

		Timestep m_TickTimer = 0.0f;

	private:
		static inline Application* s_Instance = nullptr;
	};

}