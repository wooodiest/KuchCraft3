#include "kcpch.h"
#include "Core/Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace KuchCraft {

	Application::Application(int argc, char** argv)
	{
		KC_CORE_ASSERT(s_Instance == nullptr, "Application already exists!");
		s_Instance = this;

		m_Config.Deserialize(m_ConfigPath);
		m_Config.Application.argc = argc;
		m_Config.Application.argv = argv;

		Log::SetLogLevel(LogType::Core, m_Config.Application.CoreLogLevel);
		Log::SetLogLevel(LogType::Game, m_Config.Application.GameLogLevel);

		KC_CORE_INFO("LogLevel: {}", ToString(m_Config.Application.CoreLogLevel));
		KC_INFO     ("LogLevel: {}", ToString(m_Config.Application.GameLogLevel));

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

		m_Window = CreateRef<Window>(m_Config, KC_BIND_EVENT_FN(Application::OnApplicationEvent));
		m_Window->CenterWindow();

		if (m_Config.Application.EnableImGui)
			InitImGui();

		m_Renderer = Renderer::Create(m_Config);

		m_GameLayer     = CreateRef<GameLayer>(m_Renderer, m_Config);
		m_RendererLayer = CreateRef<RendererLayer>(m_Renderer);

		m_LayerStack.AddLayer(m_GameLayer);
		m_LayerStack.AddLayer(m_RendererLayer);
	}

	Application::~Application()
	{
		s_Instance = nullptr;

		if (m_Window)
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
				/// Process fixed-time logic (Tick)
				m_TickTimer += ts;
				while (m_TickTimer >= tick_interval)
				{
					for (auto& layer : m_LayerStack)
						if (layer->ShouldUpdate())
							layer->OnTick(tick_interval);

					m_TickTimer -= tick_interval;
				}

				/// Process frame-dependent logic (Update)
				for (auto& layer : m_LayerStack)
				{
					if (layer->ShouldUpdate())
						layer->OnUpdate(ts);
				}

				/// OnRender
				m_Renderer->NewFrame();
				for (auto& layer : m_LayerStack)
				{
					if (layer->ShouldRender())
					{
						m_Renderer->SetZIndex(layer->GetZIndex());
						layer->OnRender();
					}
				}
				m_Renderer->EndFrame();

				/// ImGui
				if (m_Config.Application.EnableImGui)
				{
					ImGui_ImplOpenGL3_NewFrame();
					ImGui_ImplGlfw_NewFrame();

					ImGui::NewFrame();
					for (const auto& layer : m_LayerStack)
					{
						if (layer->ShouldRender())
							layer->OnImGuiRender();
					}
					ImGui::EndFrame();

					ImGui::Render();
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		m_LayerStack.Clear();

		if (m_Config.Application.EnableImGui)
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	}

	void Application::ProcessEvents()
	{
		Input::TransitionPressedKeys();
		Input::TransitionPressedButtons();

		m_Window->ProcessEvents();
	}

	void Application::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui_ImplGlfw_InitForOpenGL(GetWindow()->GetGLFWWindow(), true);
		std::string glslVersion = "#version " + m_Config.Renderer.GetOpenGlVersion();
		ImGui_ImplOpenGL3_Init(glslVersion.c_str());

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg].w = 0.65f;
	}

	void Application::OnApplicationEvent(ApplicationEvent& e)
	{
		if (m_Config.Application.EnableImGui && ShouldBlockEventFromImGui(e.GetEventType()))
			return;

		ApplicationEventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent> (KC_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(KC_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<KeyPressedEvent>  (KC_BIND_EVENT_FN(Application::OnKeyPressed));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;

			if (!(*it)->ShouldHandleEvents())
				continue;

			(*it)->OnApplicationEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		m_Renderer->OnWindowResize(e.GetWidth(), e.GetHeight());
		m_Minimized = e.GetWidth() == 0 || e.GetHeight() == 0;
		return false;
	}

	bool Application::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		switch (e.GetKeyCode())
		{
			case KeyCode::R: Restart(); break;
			default: break;
		}

		return false;
	}	

	bool Application::ShouldBlockEventFromImGui(ApplicationEventType type) const
	{
		if (!ImGui::GetCurrentContext())
			return false;

		ImGuiIO& io = ImGui::GetIO();
		switch (type)
		{
			case ApplicationEventType::KeyPressed:
			case ApplicationEventType::KeyReleased:
				return io.WantCaptureKeyboard;

			case ApplicationEventType::MouseButtonPressed:
			case ApplicationEventType::MouseButtonReleased:
			case ApplicationEventType::MouseMoved:
			case ApplicationEventType::MouseScrolled:
				return io.WantCaptureMouse;

			default: return false;
		}
	}
}