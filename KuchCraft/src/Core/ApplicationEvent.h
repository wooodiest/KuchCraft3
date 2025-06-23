#pragma once

#include <stdint.h>
#include <utility>
#include <string>

#include "Core/Base.h"
#include "Core/KeyCodes.h"

namespace KuchCraft {

	enum class ApplicationEventType
	{
		None = 0,

		/// Associated with window
		WindowClose, WindowResize, WindowMoved,

		/// Related to the use of the keyboard
		KeyPressed, KeyReleased, KeyTyped,

		/// Related to the use of the mouse
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum ApplicationEventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput       = BIT(1),
		EventCategoryKeyboard    = BIT(2),
		EventCategoryMouse       = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	class ApplicationEvent
	{
	public:
		virtual ~ApplicationEvent() = default;

		bool Handled = false;

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept = 0;

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept = 0;

		[[nodiscard]] bool IsInCategory(ApplicationEventCategory category) const noexcept
		{
			return GetCategoryFlags() & category;
		}

	};

	using ApplicationEventCallbackFn = std::function<void(ApplicationEvent&)>;

	/// Helper class to dispatch events based on their type. 
	class ApplicationEventDispatcher
	{
	public:
		ApplicationEventDispatcher(ApplicationEvent& event)
			: m_Event(event) {
		}

		/// Dispatches the event to the appropriate handler function.
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}

	private:
		ApplicationEvent& m_Event;

	};

	/// This event is triggered when the window is resized.
	class WindowResizeEvent : public ApplicationEvent
	{
	public:
		WindowResizeEvent(int width, int height)
			: m_Width(width), m_Height(height) {
		}

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::WindowResize; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

		inline [[nodiscard]] int GetWidth() const noexcept { return m_Width; }

		inline [[nodiscard]] int GetHeight() const noexcept { return m_Height; }

	private:
		int m_Width;
		int m_Height;

	};

	/// This event is triggered when the window is closed.
	class WindowCloseEvent : public ApplicationEvent
	{
	public:
		WindowCloseEvent() = default;

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::WindowClose; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

	};

	/// This event is triggered when the window is moved.
	class WindowMoveEvent : public ApplicationEvent
	{
	public:
		WindowMoveEvent(int x, int y)
			: m_X(x), m_Y(y) {
		}

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::WindowMoved; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryApplication; }

		inline [[nodiscard]] int GetX() const noexcept { return m_X; }

		inline [[nodiscard]] int GetY() const noexcept { return m_Y; }

	private:
		int m_X;
		int m_Y;
	};

	/// This class handles storing the key code and retrieving category flags.
	class KeyEvent : public ApplicationEvent
	{
	public:
		inline [[nodiscard]] KeyCode GetKeyCode() const noexcept { return m_KeyCode; }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryKeyboard | EventCategoryInput; }

	protected:
		KeyEvent(const KeyCode keycode)
			: m_KeyCode(keycode) {
		}

		KeyCode m_KeyCode;

	};

	/// This event is triggered when a key is pressed and stores information
	/// about whether the key press is a repeated action.
	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {
		}

		inline [[nodiscard]] bool IsRepeat() const noexcept { return m_IsRepeat; }

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::KeyPressed; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

	private:
		bool m_IsRepeat;

	};
	/// This event is triggered when a key is released.
	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {
		}

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::KeyReleased; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

	};

	/// This event is triggered when a character is typed (key press and release).
	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {
		}

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::KeyTyped; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

	};

	/// This event is triggered when the mouse is moved.
	class MouseMovedEvent : public ApplicationEvent
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {
		}

		inline [[nodiscard]] float GetX() const noexcept { return m_MouseX; }

		inline [[nodiscard]] float GetY() const noexcept { return m_MouseY; }

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::MouseMoved; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	private:
		float m_MouseX;
		float m_MouseY;

	};

	/// This event is triggered when the mouse wheel is scrolled.
	class MouseScrolledEvent : public ApplicationEvent
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {
		}

		inline [[nodiscard]] float GetXOffset() const noexcept { return m_XOffset; }

		inline [[nodiscard]] float GetYOffset() const noexcept { return m_YOffset; }

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::MouseScrolled; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput; }

	private:
		float m_XOffset;
		float m_YOffset;

	};

	/// This class stores the mouse button that triggered the event.
	class MouseButtonEvent : public ApplicationEvent
	{
	public:
		inline [[nodiscard]] MouseButton GetMouseButton() const noexcept { return m_Button; }

		virtual [[nodiscard]] int GetCategoryFlags() const noexcept override { return EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton; }

	protected:
		MouseButtonEvent(const MouseButton button)
			: m_Button(button) {
		}

		MouseButton m_Button;

	};

	/// This event is triggered when a mouse button is pressed.
	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseButton button)
			: MouseButtonEvent(button) {
		}

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::MouseButtonPressed; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept override { return GetStaticType(); }

	};

	/// This event is triggered when a mouse button is released.
	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseButton button)
			: MouseButtonEvent(button) {
		}

		static inline [[nodiscard]] ApplicationEventType GetStaticType() noexcept { return ApplicationEventType::MouseButtonReleased; }

		virtual [[nodiscard]] ApplicationEventType GetEventType() const noexcept { return GetStaticType(); }

	};

}
