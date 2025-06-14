#pragma once

#include "Core/Config.h"
#include "Core/Event.h"
#include "Core/Window.h"

namespace KuchCraft {

	class Application
	{
	public:
		Application(int argc, char** argv);

		~Application();

		void Run();

		void Close();

		static inline Application& Get() { return *s_Instance; }

		Config& GetConfig() { return m_Config; }

		Ref<Window> GetWindow() { return m_Window; }

	private:
		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		void OnShutdown();

	private:
		bool m_Running   = true;
		bool m_Minimized = false;

		Config                m_Config;
		std::filesystem::path m_ConfigPath = "config.json";

		Ref<Window> m_Window;

	private:
		static inline Application* s_Instance = nullptr;
	};

}