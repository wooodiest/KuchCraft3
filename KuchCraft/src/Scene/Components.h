#pragma once

#include "Graphics/Core/Camera.h"
#include "Graphics/Core/Texture.h"

#include "Scene/AssetManager.h"

namespace KuchCraft {

	struct IDComponent
	{
		UUID ID = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id) 
			: ID(id) {}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct RelationshipComponent
	{
		UUID ParentHandle = 0;
		std::vector<UUID> Children;

		RelationshipComponent() = default;
		RelationshipComponent(const RelationshipComponent& other) = default;
		RelationshipComponent(UUID parent)
			: ParentHandle(parent) {}
	};

	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		std::string ScriptName;

		template<typename T>
		void Bind(const std::string& scriptName)
		{
			ScriptName = scriptName;

			InstantiateScript = []() {
				return static_cast<ScriptableEntity*>(new T());
			};

			DestroyScript = [](NativeScriptComponent* nsc) {
				delete nsc->Instance;
				nsc->Instance = nullptr;
			};
		}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation    = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale       = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}
		TransformComponent(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
			: Translation(translation), Rotation(rotation), Scale(scale) {}

		inline [[nodiscard]] glm::mat4 GetTransform() const
		{
			return glm::translate(glm::mat4(1.0f), Translation)
				* glm::toMat4(glm::quat(Rotation))
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct CameraComponent
	{
		Camera Camera;

		/// Flag indicating if the camera's aspect ratio should remain fixed.
		bool FixedAspectRatio = false;

		/// Flag indicating if the camera automaticly should set position and rotation from transform component
		bool UseTransformComponent = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		AssetHandle Asset;

		float TilingFactor = 1.0f;
		glm::vec2 UVStart  = { 0.0f, 0.0f };
		glm::vec2 UVEnd    = { 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& other) = default;
	};

	struct PlaneRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

		AssetHandle Asset;

		float TilingFactor = 1.0f;
		glm::vec2 UVStart = { 0.0f, 0.0f };
		glm::vec2 UVEnd = { 1.0f, 1.0f };

		PlaneRendererComponent() = default;
		PlaneRendererComponent(const PlaneRendererComponent& other) = default;
	};

}