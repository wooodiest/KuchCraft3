#pragma once

#include "Config.h"

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

	private:
		bool m_Running   = true;
		bool m_Minimized = false;

		Config                m_Config;
		std::filesystem::path m_ConfigPath = "config.json";

	private:
		static inline Application* s_Instance = nullptr;
	};

}