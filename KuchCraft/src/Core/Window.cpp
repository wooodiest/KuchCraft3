#include "kcpch.h"
#include "Window.h"

#include <glad/glad.h>
#include <stb_image.h>

namespace KuchCraft {

	static void GLFWErrorCallback(int error, const char* description)
	{
		Log::PrintMessage(LogType::Core, LogLevel::Error, "[GLFW]: {0} : {1}", error, description);
	}

	Window::Window(const WindowConfig& config, EventCallbackFn callback)
		: m_Data(config, callback)
	{
		glfwSetErrorCallback(GLFWErrorCallback);

		if (!glfwInit())
		{
			KC_CORE_ERROR("Failed to initialize GLFW"); 
		}

		KC_TODO("Set up GLFW window hints based on config");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_RESIZABLE, m_Data.Config.Resizable ? GLFW_TRUE : GLFW_FALSE);

		if (m_Data.Config.Width < min_window_width || m_Data.Config.Height < min_window_height ||
			m_Data.Config.Width > max_window_width || m_Data.Config.Height > max_window_height)
		{
			KC_CORE_WARN("Window size is out of bounds, clamping to min values");
			m_Data.Config.Width  = min_window_width;
			m_Data.Config.Height = min_window_height;
		}

		m_Window = glfwCreateWindow(m_Data.Config.Width, m_Data.Config.Height, m_Data.Config.Title.c_str(), nullptr, nullptr);

		KC_CORE_ASSERT(m_Window, "Failed to create GLFW window");
		if (!m_Window)
		{
			glfwTerminate();
			KC_CORE_FATAL("Failed to create GLFW window");
		}

		if (!m_Data.Config.IconPath.empty())
		{
			GLFWimage icon;
			icon.pixels = stbi_load(m_Data.Config.IconPath.string().c_str(), &icon.width, &icon.height, nullptr, 4);
			if (icon.pixels)
			{
				glfwSetWindowIcon(m_Window, 1, &icon);
				stbi_image_free(icon.pixels);
			}
			else
			{
				KC_CORE_WARN("Failed to load window icon from path: {0}", m_Data.Config.IconPath.string());
			}
		}

		SetFullScreen(m_Data.Config.FullScreen);

		glfwSetWindowSizeLimits(m_Window, min_window_width, min_window_height, max_window_width, max_window_height);
		glfwSetWindowPos(m_Window, m_Data.Config.PositionX, m_Data.Config.PositionY);

		glfwMakeContextCurrent(m_Window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			glfwDestroyWindow(m_Window);
			glfwTerminate();
			KC_CORE_FATAL("Failed to initialize OpenGL context");
		}

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwGetWindowContentScale(m_Window, &m_Data.Config.ContentScaleX, &m_Data.Config.ContentScaleY);

		SetVSync(m_Data.Config.VSync);
		SetCursorMode(m_Data.Config.CursorMode);

		/// Set up event callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.Config.Width  = width;
			data.Config.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xPos, int yPos) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
			data.Config.PositionX = xPos;
			data.Config.PositionY = yPos;

			WindowMoveEvent event(xPos, yPos);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Pressed);
					KeyPressedEvent event(static_cast<KeyCode>(key), false);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Released);
					KeyReleasedEvent event(static_cast<KeyCode>(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					Input::UpdateKeyState((KeyCode)key, KeyState::Held);
					KeyPressedEvent event(static_cast<KeyCode>(key), true);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			KeyTypedEvent event(static_cast<KeyCode>(keycode));
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			switch (action)
			{
				case GLFW_PRESS:
				{
					Input::UpdateButtonState((MouseButton)button, KeyState::Pressed);
					MouseButtonPressedEvent event(static_cast<MouseButton>(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					Input::UpdateButtonState((MouseButton)button, KeyState::Released);
					MouseButtonReleasedEvent event(static_cast<MouseButton>(button));
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

			MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
			data.EventCallback(event);
		});
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::Update()
	{
		float time = (float)glfwGetTime();
		m_TimeData.DeltaTime    = time - m_TimeData.LastFrameTime;
		m_TimeData.RawDeltaTime = m_TimeData.DeltaTime;
		m_TimeData.DeltaTime    = m_TimeData.DeltaTime > max_delta_time ? max_delta_time : m_TimeData.DeltaTime;

		/// Update the last frame time to the current time for the next frame calculation.
		m_TimeData.LastFrameTime = time;

		KC_TODO("Update mouse data in window based on Mouse Input");
		/// glm::vec2 position = Input::GetMousePosition();
		/// m_MouseData.PositionDifference = position - m_MouseData.PreviousPosition;
		/// m_MouseData.PreviousPosition   = position;
	}

	void Window::ProcessEvents()
	{
		glfwPollEvents();
	}

	void Window::SwapBuffers()
	{
		glfwSwapBuffers(m_Window);
	}

	void Window::SetSize(int width, int height)
	{
		if (width < min_window_width || height < min_window_height ||
			width > max_window_width || height > max_window_height)
		{
			KC_CORE_WARN("Window size is out of bounds, clamping to min values");
			width  = std::max(min_window_width, width);
			height = std::max(min_window_height, height);
		}

		m_Data.Config.Width  = width;
		m_Data.Config.Height = height;

		glfwSetWindowSize(m_Window, width, height);
		KC_CORE_INFO("Window size set to ({0}, {1})", width, height);
	}

	void Window::SetPosition(int x, int y)
	{
		m_Data.Config.PositionX = x;
		m_Data.Config.PositionY = y;

		glfwSetWindowPos(m_Window, x, y);
		KC_CORE_INFO("Window position set to ({0}, {1})", x, y);
	}

	void Window::SetFullScreen(bool enabled)
	{
		if (enabled)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);

			m_Data.Config.FullScreen = true;
			KC_CORE_INFO("Window set to full screen");
		}
		else
		{
			glfwSetWindowMonitor(m_Window, nullptr, m_Data.Config.PositionX, m_Data.Config.PositionY, m_Data.Config.Width, m_Data.Config.Height, 0);

			m_Data.Config.FullScreen = false;
			KC_CORE_INFO("Window set to windowed mode");
		}
	}

	void Window::SetBorderless(bool enabled)
	{
		m_Data.Config.Borderless = enabled;

		glfwSetWindowAttrib(m_Window, GLFW_DECORATED, enabled ? GLFW_TRUE : GLFW_FALSE);
		KC_CORE_INFO("Window borderless mode set to {0}", enabled ? "enabled" : "disabled");
	}

	void Window::SetVSync(bool enabled)
	{
		glfwSwapInterval(enabled ? 1 : 0);

		m_Data.Config.VSync = enabled;
		KC_CORE_INFO("VSync set to {0}", enabled ? "enabled" : "disabled");
	}

	void Window::SetResizable(bool resizable)
	{
		glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

		m_Data.Config.Resizable = resizable;
		KC_CORE_INFO("Window resizable set to {0}", resizable ? "true" : "false");
	}

	void Window::SetCursorMode(WindowCursorMode mode)
	{
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);

		m_Data.Config.CursorMode = mode;
		KC_CORE_INFO("Cursor mode set to {0}", mode == WindowCursorMode::Normal ? "Normal" :
			mode == WindowCursorMode::Hidden ? "Hidden" : "Disabled");
	}

	void Window::SetTitle(const std::string& title)
	{
		if (title.empty())
		{
			KC_CORE_WARN("Window title cannot be empty");
			return;
		}

		m_Data.Config.Title = title;
		glfwSetWindowTitle(m_Window, title.c_str());

		KC_CORE_INFO("Window title set to: {0}", title);
	}

	void Window::Minimize()
	{
		glfwIconifyWindow(m_Window);

		KC_CORE_INFO("Window minimized");
	}

	void Window::Maximize()
	{
		glfwMaximizeWindow(m_Window);

		KC_CORE_INFO("Window maximized");
	}

	void Window::Restore()
	{
		glfwRestoreWindow(m_Window);

		KC_CORE_INFO("Window restored");
	}

	void Window::CenterWindow()
	{
		const GLFWvidmode* videmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		int x = (videmode->width  / 2) - (m_Data.Config.Width  / 2);
		int y = (videmode->height / 2) - (m_Data.Config.Height / 2);

		SetPosition(x, y);
	}

	bool Window::IsFocused() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED);
	}

	bool Window::IsMinimized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED);
	}

	bool Window::IsMaximized() const
	{
		return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED);
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Window);
	}

}