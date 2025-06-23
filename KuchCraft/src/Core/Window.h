#pragma once

#include <GLFW/glfw3.h>

namespace KuchCraft {

	class Window
	{
	public:
		Window(const WindowConfig& config, ApplicationEventCallbackFn callback);
		~Window();

		void Update();
		void ProcessEvents();
		void SwapBuffers();

		int GetWidth()  const { return m_Data.Config.Width; }
		int GetHeigth() const { return m_Data.Config.Height; }
		std::pair<int, int> GetSize() const { return { m_Data.Config.Width, m_Data.Config.Height }; }

		int GetPositionX() const { return m_Data.Config.PositionX; }
		int GetPositionY() const { return m_Data.Config.PositionY; }
		std::pair<int, int> GetPosition() const { return { m_Data.Config.PositionX, m_Data.Config.PositionY }; }

		Timestep GetDeltaTime()     const { return m_TimeData.DeltaTime;     }
		Timestep GetRawDeltaTime()  const { return m_TimeData.RawDeltaTime;  }
		Timestep GetLastFrameTime() const { return m_TimeData.LastFrameTime; }

		std::pair<float, float> GetMousePositionDifference() const { return m_MouseData.PositionDifference; }
		std::pair<float, float> GetMousePreviousPosition()   const { return m_MouseData.PreviousPosition; }

		void SetSize(int width, int height);
		void SetWidth (int width)  { SetSize(width, m_Data.Config.Height); }
		void SetHeight(int height) { SetSize(m_Data.Config.Width, height); }

		void SetPosition(int x, int y);
		void SetPositionX(int x) { SetPosition(x, m_Data.Config.PositionY); }
		void SetPositionY(int y) { SetPosition(m_Data.Config.PositionX, y); }

		void SetFullScreen(bool enabled);
		bool IsFullScreen() const { return m_Data.Config.FullScreen; }

		void SetBorderless(bool enabled);
		bool IsBorderless() const { return m_Data.Config.Borderless; }

		void SetVSync(bool enabled);
		bool IsVSync() const { return m_Data.Config.VSync; };

		void SetResizable(bool resizable);
		bool IsResizable() const { return m_Data.Config.Resizable; }

		void SetCursorMode(WindowCursorMode mode);
		WindowCursorMode GetCursorMode() const { return m_Data.Config.CursorMode; }

		void SetTitle(const std::string& title);
		const std::string& GetTitle() const { return m_Data.Config.Title; }

		void Minimize();
		void Maximize();
		void Restore();
		void CenterWindow();

		bool IsFocused()   const;
		bool IsMinimized() const;
		bool IsMaximized() const;
		bool ShouldClose() const;

		inline const WindowConfig& GetConfig() const { return m_Data.Config; }

		inline GLFWwindow* GetGLFWWindow() const { return m_Window; }

	private:
		struct WindowData
		{
			WindowConfig Config;
			ApplicationEventCallbackFn EventCallback;
		} m_Data;

		GLFWwindow*  m_Window = nullptr;
		
		struct TimeData {
			Timestep DeltaTime     = 0.0f; /// Time interval between the current frame and the last frame (clamped to max_delta_time)
			Timestep RawDeltaTime  = 0.0f; /// Time interval between the current frame and the last frame	
			Timestep LastFrameTime = 0.0f; /// Time of the last frame, used for calculating the delta time.
		} m_TimeData;

		struct MouseData {		
			std::pair<float, float> PositionDifference{ 0.0f, 0.0f }; /// The mouse position difference between frames
			std::pair<float, float> PreviousPosition  { 0.0f, 0.0f }; /// The mouse position in previous frame
		} m_MouseData;
	};

}