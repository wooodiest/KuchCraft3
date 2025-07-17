#include "kcpch.h"
#include "KuchCraft/CameraController.h"

#include "Core/Application.h"

#include <imgui/imgui.h>

namespace KuchCraft {

	void CameraController::OnCreate()
	{
		if (!HasComponent<TransformComponent>())
			AddComponent<TransformComponent>();

		if (!HasComponent<CameraComponent>())
			AddComponent<CameraComponent>();
	}

	void CameraController::OnUpdate(Timestep ts)
	{
		auto& cameraComponent = GetComponent<CameraComponent>();
		auto& transformComponent = GetComponent<TransformComponent>();

		auto [positionDiffX, positionDiffY] = Application::Get().GetWindow()->GetMousePositionDifference();

		transformComponent.Rotation.x += positionDiffX * m_MouseSensitivity * 0.001f;
		transformComponent.Rotation.y -= positionDiffY * m_MouseSensitivity * 0.001f;

		constexpr float min_pitch = glm::radians(-89.9f);
		constexpr float max_pitch = glm::radians(89.9f);
		if (transformComponent.Rotation.y < min_pitch)
			transformComponent.Rotation.y = min_pitch;
		if (transformComponent.Rotation.y > max_pitch)
			transformComponent.Rotation.y = max_pitch;

		constexpr float yaw_boundary = glm::radians(360.0f);
		if (transformComponent.Rotation.x > yaw_boundary)
			transformComponent.Rotation.x -= yaw_boundary;
		if (transformComponent.Rotation.x < 0.0f)
			transformComponent.Rotation.x += yaw_boundary;

		if (Input::IsKeyPressed(KeyCode::W))
			transformComponent.Translation += cameraComponent.Camera.GetForwardDirection() * m_MovementSpeed * ts.GetSeconds();
		if (Input::IsKeyPressed(KeyCode::S))
			transformComponent.Translation -= cameraComponent.Camera.GetForwardDirection() * m_MovementSpeed * ts.GetSeconds();

		if (Input::IsKeyPressed(KeyCode::A))
			transformComponent.Translation -= cameraComponent.Camera.GetRightDirection() * m_MovementSpeed * ts.GetSeconds();
		if (Input::IsKeyPressed(KeyCode::D))
			transformComponent.Translation += cameraComponent.Camera.GetRightDirection() * m_MovementSpeed * ts.GetSeconds();

		if (Input::IsKeyPressed(KeyCode::LeftControl))
			transformComponent.Translation -= cameraComponent.Camera.GetUpDirection() * m_MovementSpeed * ts.GetSeconds();
		if (Input::IsKeyPressed(KeyCode::Space))
			transformComponent.Translation += cameraComponent.Camera.GetUpDirection() * m_MovementSpeed * ts.GetSeconds();

		cameraComponent.Camera.UpdateTransform(transformComponent.Translation, transformComponent.Rotation);
	}

	void CameraController::OnSerialize(nlohmann::json& state)
	{
		state["MouseSensitivity"] = m_MouseSensitivity;
		state["MovementSpeed"]    = m_MovementSpeed;
	}

	void CameraController::OnDeserialize(const nlohmann::json& state)
	{
		if (state.contains("MouseSensitivity"))
			m_MouseSensitivity = state["MouseSensitivity"].get<float>();

		if (state.contains("MovementSpeed"))
			m_MovementSpeed = state["MovementSpeed"].get<float>();
	}

	void CameraController::OnImGuiHierarchyPanel()
	{
		ImGui::DragFloat("Mouse sensitivity", &m_MouseSensitivity, 0.05f);
		ImGui::DragFloat("Movement speed", &m_MovementSpeed, 0.5f);
	}

}
