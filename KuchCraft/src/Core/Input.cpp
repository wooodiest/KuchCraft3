#include "kcpch.h"
#include "Core/Input.h"

#include "Core/Application.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace KuchCraft {

	bool Input::IsKeyPressed(KeyCode keycode)
	{
		return s_KeyData.find(keycode) != s_KeyData.end() && s_KeyData[keycode].State == KeyState::Pressed;
	}

	bool Input::IsKeyHeld(KeyCode keycode)
	{
		return s_KeyData.find(keycode) != s_KeyData.end() && s_KeyData[keycode].State == KeyState::Held;
	}

	bool Input::IsKeyDown(KeyCode keycode)
	{
		bool enableImGui = Application::Get().GetConfig().Application.EnableImGui;
		if (enableImGui && ImGui::GetIO().WantCaptureKeyboard)
			return false;

		GLFWwindow* window = Application::Get().GetWindow()->GetGLFWWindow();
		auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyReleased(KeyCode keycode)
	{
		return s_KeyData.find(keycode) != s_KeyData.end() && s_KeyData[keycode].State == KeyState::Released;
	}

	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		return s_MouseData.find(button) != s_MouseData.end() && s_MouseData[button].State == KeyState::Pressed;
	}

	bool Input::IsMouseButtonHeld(MouseButton button)
	{
		return s_MouseData.find(button) != s_MouseData.end() && s_MouseData[button].State == KeyState::Held;
	}

	bool Input::IsMouseButtonDown(MouseButton button)
	{
		bool enableImGui = Application::Get().GetConfig().Application.EnableImGui;
		if (enableImGui && ImGui::GetIO().WantCaptureMouse)
			return false;

		GLFWwindow* window = Application::Get().GetWindow()->GetGLFWWindow();
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonReleased(MouseButton button)
	{
		return s_MouseData.find(button) != s_MouseData.end() && s_MouseData[button].State == KeyState::Released;
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return static_cast<float>(x);
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return static_cast<float>(y);
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		GLFWwindow* window = Application::Get().GetWindow()->GetGLFWWindow();

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		return { static_cast<float>(x), static_cast<float>(y) };
	}

	void Input::TransitionPressedKeys()
	{
		for (const auto& [key, keyData] : s_KeyData)
		{
			if (keyData.State == KeyState::Pressed)
				UpdateKeyState(key, KeyState::Held);
		}
	}

	void Input::TransitionPressedButtons()
	{
		for (const auto& [button, buttonData] : s_MouseData)
		{
			if (buttonData.State == KeyState::Pressed)
				UpdateButtonState(button, KeyState::Held);
		}
	}

	void Input::UpdateKeyState(KeyCode keycode, KeyState newState)
	{
		auto& keyData = s_KeyData[keycode];

		keyData.Key = keycode;
		keyData.OldState = keyData.State;
		keyData.State = newState;
	}

	void Input::UpdateButtonState(MouseButton button, KeyState newState)
	{
		auto& mouseData = s_MouseData[button];

		mouseData.Button = button;
		mouseData.OldState = mouseData.State;
		mouseData.State = newState;
	}

	void Input::ClearReleasedKeys()
	{
		for (const auto& [key, keyData] : s_KeyData)
		{
			if (keyData.State == KeyState::Released)
				UpdateKeyState(key, KeyState::None);
		}

		for (const auto& [button, buttonData] : s_MouseData)
		{
			if (buttonData.State == KeyState::Released)
				UpdateButtonState(button, KeyState::None);
		}
	}

}