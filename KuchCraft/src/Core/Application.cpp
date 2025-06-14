#include "kcpch.h"
#include "Core/Application.h"

namespace KuchCraft {

	Application::Application(int argc, char** argv)
	{
		s_Instance = this;

		m_Config.Deserialize(m_ConfigPath);
		m_Config.Application.argc = argc;
		m_Config.Application.argv = argv;

		if (argc > 1)
		{
			KC_CORE_WARN("More than one word file path specified, using {}", argv[0]);
		}

	}

	Application::~Application()
	{
		s_Instance = nullptr;
		m_Config.Serialize(m_ConfigPath);
	}

	void Application::Run()
	{
		KC_CORE_INFO("Running KuchCraft {}", KC_VERSION_LONG);
	}

	void Application::Close()
	{
		m_Running = false;
	}

}