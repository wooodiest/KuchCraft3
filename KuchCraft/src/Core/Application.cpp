#include "kcpch.h"
#include "Core/Application.h"

namespace KuchCraft {

	Application::Application(int argc, char** argv)
	{
		KC_CORE_ASSERT(s_Instance == nullptr, "Application already exists!");
		s_Instance = this;

		m_Config.Deserialize(m_ConfigPath);
		m_Config.Application.argc = argc;
		m_Config.Application.argv = argv;

		if (argc > 1)
		{
			KC_CORE_INFO("Received {} argument(s):", argc - 1);
			for (int i = 1; i < argc; i++)
			{
				KC_CORE_INFO("[{}]: {}", i, argv[i]);
			}
		}
		else
		{
			KC_CORE_INFO("No command-line arguments provided.");
		}

		m_Window = CreateRef<Window>(m_Config.Window, KC_BIND_EVENT_FN(Application::OnEvent));
		m_Window->CenterWindow();
	}

	Application::~Application()
	{
		s_Instance = nullptr;

		m_Config.Window = m_Window->GetConfig();
		m_Config.Serialize(m_ConfigPath);
	}

	void Application::Run()
	{
		KC_CORE_INFO("Running KuchCraft {}", KC_VERSION_LONG);

		while (m_Running)
		{
			m_Window->Update();
			ProcessEvents();

			Timestep ts = m_Window->GetDeltaTime();
			if (!m_Minimized)
			{
				if (Input::IsKeyPressed(KeyCode::R))
				{
					Restart();
				}
			}

			m_Window->SwapBuffers();

			Input::ClearReleasedKeys();
		}

		OnShutdown();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Restart()
	{
		m_Restart = true;
		m_Running = false;
	}

	void Application::OnShutdown()
	{
	}

	void Application::ProcessEvents()
	{
		Input::TransitionPressedKeys();
		Input::TransitionPressedButtons();

		m_Window->ProcessEvents();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent> (KC_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(KC_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>  (KC_BIND_EVENT_FN(Application::OnKeyPressed));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_Minimized = e.GetWidth() == 0 || e.GetHeight() == 0;
		return false;
	}

	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		return false;
	}	

}