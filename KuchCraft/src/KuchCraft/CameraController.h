#pragma once

#include "Scene/ScriptableEntity.h"

namespace KuchCraft {

	class CameraController : public ScriptableEntity
	{
	public:
		virtual void OnCreate() override;
		virtual void OnUpdate(Timestep ts) override;

		virtual void OnSerialize(nlohmann::json& state)  override;
		virtual void OnDeserialize(const nlohmann::json& state)  override;

		virtual void OnImGuiHierarchyPanel() override;

	private:
		float m_MouseSensitivity = 0.25f;
		float m_MovementSpeed = 1.0f;

	};

	REGISTER_NATIVE_SCRIPT(CameraController);

}