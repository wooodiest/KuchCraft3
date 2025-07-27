#include "kcpch.h"
#include "Scene/SceneSerializer.h"

#include <nlohmann_json/json.hpp>

#include "Scene/Entity.h"
#include "Scene/ScriptableEntity.h"
#include "Scene/Components.h"

namespace KuchCraft {

	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{
		
	}

	bool SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		if (!m_Scene)
		{
			KC_CORE_ERROR("Invalid Scene, cannot serialize!");
			return false;
		}

		if (filepath.empty())
		{
			KC_CORE_ERROR("Invalid path, cannot serialize scene!");
			return false;
		}

		nlohmann::json sceneJson;

		sceneJson["SceneUUID"] = m_Scene->GetUUID();

		Entity primaryCameraEntity = m_Scene->GetPrimaryCameraEntity();
		if (primaryCameraEntity)
			sceneJson["PrimaryCameraEntityUUID"] = primaryCameraEntity.GetUUID();

		sceneJson["DataPack"] = m_Scene->GetDataPackName();

		const auto entityMap = m_Scene->GetEntityMap();
		for (const auto& [uuid, entity] : entityMap)
		{
			nlohmann::json entityJson;

			if (entity.HasComponent<IDComponent>())
			{
				entityJson["UUID"] = entity.GetComponent<IDComponent>().ID;
			}

			if (entity.HasComponent<TagComponent>())
			{
				entityJson["Tag"] = entity.GetComponent<TagComponent>().Tag;
			}		

			if (entity.HasComponent<RelationshipComponent>())
			{
				auto& ralationship = entity.GetComponent<RelationshipComponent>();
				entityJson["Relationship"] = {
					{ "Parent",   ralationship.ParentHandle },
					{ "Children", ralationship.Children},
				};
			}

			if (entity.HasComponent<NativeScriptComponent>())
			{
				auto& script = entity.GetComponent<NativeScriptComponent>();
				entityJson["NativeScript"] = {
					{ "ScriptName", script.ScriptName }
				};

				if (script.Instance)
				{
					nlohmann::json stateJson;
					script.Instance->OnSerialize(stateJson);
					entityJson["NativeScript"]["State"] = stateJson;
				}
			}

			if (entity.HasComponent<TransformComponent>())
			{
				auto& transform = entity.GetComponent<TransformComponent>();
				entityJson["Transform"] = {
					{ "Translation", transform.Translation },
					{ "Rotation",    transform.Rotation },
					{ "Scale",       transform.Scale }
				};
			}

			if (entity.HasComponent<CameraComponent>())
			{
				auto& camera = entity.GetComponent<CameraComponent>();
				entityJson["Camera"] = {
					{ "FixedAspectRatio",      camera.FixedAspectRatio        },
					{ "UseTransformComponent", camera.UseTransformComponent   },
					{ "Fov",		           camera.Camera.GetFov()         },
					{ "AspectRatio",		   camera.Camera.GetAspectRatio() },
					{ "NearClip",			   camera.Camera.GetNearClip()    },
					{ "FarClip",			   camera.Camera.GetFarClip()     },
				};
			}

			if (entity.HasComponent<SpriteRendererComponent>())
			{
				auto& sprite = entity.GetComponent<SpriteRendererComponent>();
				entityJson["SpriteRenderer"] = {
					{ "Color",        sprite.Color },
					{ "TilingFactor", sprite.TilingFactor },
					{ "UVStart",      sprite.UVStart },
					{ "UVEnd",        sprite.UVEnd   },
					{ "Asset",        sprite.Asset.ID }
				};
			}

			if (entity.HasComponent<PlaneRendererComponent>())
			{
				auto& plane = entity.GetComponent<PlaneRendererComponent>();
				entityJson["PlaneRenderer"] = {
					{ "Color",        plane.Color },
					{ "TilingFactor", plane.TilingFactor },
					{ "UVStart",      plane.UVStart },
					{ "UVEnd",        plane.UVEnd   },
					{ "Asset",        plane.Asset.ID }
				};
			}

			sceneJson["Entities"].push_back(entityJson);
		}

		std::ofstream file(filepath);
		if (!file.is_open())
		{
			KC_CORE_ERROR("Failed to open : {}", filepath.string());
			return false;
		}

		file << sceneJson.dump(4, ' ');
		file.close();

		return true;
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			KC_CORE_WARN("File does not exist: {}", filepath.string());
			return false;
		}

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			KC_CORE_ERROR("Failed to open file: {}", filepath.string());
			return false;
		}

		nlohmann::json sceneJson;
		try
		{
			file >> sceneJson;
			file.close();
		}
		catch (std::exception& e)
		{
			KC_CORE_ERROR("Failed to parse JSON: {}", e.what());
			return false;
		}

		if (sceneJson.contains("SceneUUID"))
			m_Scene->m_SceneID = sceneJson["SceneUUID"].get<UUID>();

		if (sceneJson.contains("DataPackName"))
			m_Scene->m_DataPackName = sceneJson["DataPackName"].get<std::string>();

		m_Scene->DestroyAllEntities();
		for (auto& entityJson : sceneJson["Entities"])
		{
			std::string tag = "Unnamed";
			UUID entityUUID = 0;

			if (entityJson.contains("UUID"))
				entityUUID = entityJson["UUID"].get<UUID>();
			else
			{
				KC_CORE_WARN("Serialized entity does not have an UUID");
			}

			if (entityJson.contains("Tag"))
				tag = entityJson["Tag"].get<std::string>();
			else
			{
				KC_CORE_WARN("Serialized entity does not have a Tag");
			}

			Entity entity = m_Scene->CreateEntityWithUUID(entityUUID, tag);

			if (entityJson.contains("Relationship"))
			{
				nlohmann::json relationshipJson = entityJson["Relationship"];
				
				if (relationshipJson.contains("Parent"))
					entity.SetParentUUID(relationshipJson["Parent"].get<UUID>());

				if (relationshipJson.contains("Children") && !relationshipJson["Children"].is_null())
				{
					for (const auto& child : relationshipJson["Children"])
						entity.AddChildUUID(child.get<UUID>());
				}
			}
			else
			{
				KC_CORE_WARN("Serialized entity does not have a Relationship");
			}

			if (entityJson.contains("NativeScript"))
			{
				nlohmann::json nativeScriptJson = entityJson["NativeScript"];
				if (nativeScriptJson.contains("ScriptName"))
				{
					const auto& scriptName = nativeScriptJson["ScriptName"];
					auto& nsc = entity.AddComponent<NativeScriptComponent>();

					if (NativeScriptRegistry::BindScript(scriptName, nsc))
					{
						if (nativeScriptJson.contains("State") && !nativeScriptJson["State"].is_null())
						{
							nsc.Instance = nsc.InstantiateScript();
							nsc.Instance->m_Entity = entity;
							nsc.Instance->OnDeserialize(nativeScriptJson["State"]);
						}
					}
					else
					{
						KC_CORE_WARN("NativeScript not found: {}", scriptName);
					}
				}
				else
				{
					KC_CORE_WARN("Entity has NativeScriptComponent but no ScriptName");
				}
			}

			if (entityJson.contains("Transform"))
			{
				auto& transformComponent = entity.AddComponent<TransformComponent>();

				nlohmann::json transformJson = entityJson["Transform"];
				if (transformJson.contains("Translation"))
					transformComponent.Translation = transformJson["Translation"].get<glm::vec3>();
				if (transformJson.contains("Rotation"))
					transformComponent.Rotation = transformJson["Rotation"].get<glm::vec3>();
				if (transformJson.contains("Scale"))
					transformComponent.Scale = transformJson["Scale"].get<glm::vec3>();
			}

			if (entityJson.contains("Camera"))
			{
				auto& cameraComponent = entity.AddComponent<CameraComponent>();

				nlohmann::json cameraJson = entityJson["Camera"];
				if (cameraJson.contains("FixedAspectRatio"))
					cameraComponent.FixedAspectRatio = cameraJson["FixedAspectRatio"];
				if (cameraJson.contains("UseTransformComponent"))
					cameraComponent.UseTransformComponent = cameraJson["UseTransformComponent"];
				if (cameraJson.contains("Fov"))
					cameraComponent.Camera.SetFov(cameraJson["Fov"]);
				if (cameraJson.contains("AspectRatio"))
					cameraComponent.Camera.SetAspectRatio(cameraJson["AspectRatio"]);
				if (cameraJson.contains("NearClip"))
					cameraComponent.Camera.SetNearClip(cameraJson["NearClip"]);
				if (cameraJson.contains("FarClip"))
					cameraComponent.Camera.SetFarClip(cameraJson["FarClip"]);
			}

			if (entityJson.contains("SpriteRenderer"))
			{
				auto& spriteRendererComponent = entity.AddComponent<SpriteRendererComponent>();

				nlohmann::json spriteRendererJson = entityJson["SpriteRenderer"];
				if (spriteRendererJson.contains("Color"))
					spriteRendererComponent.Color = spriteRendererJson["Color"];
				if (spriteRendererJson.contains("TilingFactor"))
					spriteRendererComponent.TilingFactor = spriteRendererJson["TilingFactor"];
				if (spriteRendererJson.contains("UVStart"))
					spriteRendererComponent.UVStart = spriteRendererJson["UVStart"];
				if (spriteRendererJson.contains("UVEnd"))
					spriteRendererComponent.UVEnd = spriteRendererJson["UVEnd"];
				if (spriteRendererJson.contains("Asset"))
					spriteRendererComponent.Asset.ID = spriteRendererJson["Asset"];
			}

			if (entityJson.contains("PlaneRenderer"))
			{
				auto& planeRendererComponent = entity.AddComponent<PlaneRendererComponent>();

				nlohmann::json planeRendererJson = entityJson["PlaneRenderer"];
				if (planeRendererJson.contains("Color"))
					planeRendererComponent.Color = planeRendererJson["Color"];
				if (planeRendererJson.contains("TilingFactor"))
					planeRendererComponent.TilingFactor = planeRendererJson["TilingFactor"];
				if (planeRendererJson.contains("UVStart"))
					planeRendererComponent.UVStart = planeRendererJson["UVStart"];
				if (planeRendererJson.contains("UVEnd"))
					planeRendererComponent.UVEnd = planeRendererJson["UVEnd"];
				if (planeRendererJson.contains("Asset"))
					planeRendererComponent.Asset.ID = planeRendererJson["Asset"];
			}
		}

		if (sceneJson.contains("PrimaryCameraEntityUUID"))
		{
			Entity primaryCameraEntity = m_Scene->TryGetEntityWithUUID(sceneJson["PrimaryCameraEntityUUID"].get<UUID>());
			if (primaryCameraEntity)
				m_Scene->SetPrimaryCamera(primaryCameraEntity);
		}

		return true;
	}
}
