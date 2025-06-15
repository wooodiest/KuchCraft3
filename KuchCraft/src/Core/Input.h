#pragma once

#include "Core/KeyCodes.h"

namespace KuchCraft {

	struct KeyData
	{
		KeyCode  Key;
		KeyState State    = KeyState::None;
		KeyState OldState = KeyState::None;
	};

	struct ButtonData
	{
		MouseButton Button;
		KeyState    State    = KeyState::None;
		KeyState    OldState = KeyState::None;
	};

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsKeyHeld(KeyCode keycode);
		static bool IsKeyDown(KeyCode keycode);
		static bool IsKeyReleased(KeyCode keycode);

		static bool IsMouseButtonPressed(MouseButton button);
		static bool IsMouseButtonHeld(MouseButton button);
		static bool IsMouseButtonDown(MouseButton button);
		static bool IsMouseButtonReleased(MouseButton button);

		static float GetMouseX();
		static float GetMouseY();
		static std::pair<float, float> GetMousePosition();

		/// Internal use only...
		static void TransitionPressedKeys();
		static void TransitionPressedButtons();
		static void UpdateKeyState(KeyCode keycode, KeyState newState);
		static void UpdateButtonState(MouseButton button, KeyState newState);
		static void ClearReleasedKeys();

	private:
		inline static std::map<KeyCode,     KeyData   > s_KeyData;
		inline static std::map<MouseButton, ButtonData> s_MouseData;
	};

}