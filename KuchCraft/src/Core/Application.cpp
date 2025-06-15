#include "kcpch.h"
#include "Core/Application.h"

#include "Core/Event.h"
#include "Core/Input.h"

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

			float deltaTime = m_Window->GetDeltaTime();
			if (!m_Minimized)
			{
			}

			m_Window->SwapBuffers();

			Input::ClearReleasedKeys();
		}

	}

	void Application::Close()
	{
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