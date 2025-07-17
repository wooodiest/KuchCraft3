#include "kcpch.h"
#include "KuchCraft/GameLayer.h"

#include "Core/Application.h"
#include "Scene/Entity.h"
#include "KuchCraft/CameraController.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace KuchCraft {

	GameLayer::GameLayer(const Ref<Renderer>& renderer, Config config)
		: Layer("GameLayer", LayerType::Game), m_Renderer(renderer), m_Config(config)
	{
		if (config.Application.argc > 1 && config.Application.argv[1])
			LoadWorld(config.Application.argv[1]);
	}

	GameLayer::~GameLayer()
	{
	}

	void GameLayer::OnAttach()
	{
	}

	void GameLayer::OnDetach()
	{
	}

	void GameLayer::OnUpdate(Timestep ts)
	{
		if (m_Scene)
			m_Scene->OnUpdate(ts);
	}

	void GameLayer::OnTick(const Timestep ts)
	{
		if (m_Scene)
			m_Scene->OnTick(ts);
	}

	void GameLayer::OnRender()
	{
		if (m_Scene)
			m_Scene->OnRender();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		if (!entity.HasComponent<T>())
			return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;

		if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s", name.c_str()))
		{
			uiFunction(entity.GetComponent<T>());
			ImGui::TreePop();
		}
	}

	void GameLayer::OnImGuiRender()
	{
		/// ...

		if (!m_Scene)
			return;

		ImGui::Begin("Game Debug Tools");
		constexpr float margin = 6.0f;

		if (ImGui::CollapsingHeader("Scene##GameLayer", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SeparatorText("Entities List");

			constexpr float entities_list_height = 250.0f;
			ImGui::BeginChild("EntitiesList", ImVec2(0.0f, entities_list_height), true);

			const auto& enities = m_Scene->GetEntityMap();
			for (const auto& [uuid, entity] : enities)
			{
				if (!entity.GetParent())
					DrawEntityNode(entity);
			}
			ImGui::EndChild();

			Entity selectedEntity = m_Scene->TryGetEntityWithUUID(m_HierarchyPanelSelectedEntity);
			if (selectedEntity)
			{
				ImGui::SeparatorText("Entity info");
				ImGui::Text("UUID: %llu", selectedEntity.GetUUID());
				ImGui::Text("Tag: %s", selectedEntity.GetTag());

				DrawComponent<TagComponent>("Tag Component", selectedEntity, [](auto& tag) {
					std::string entityTag = tag.Tag;
					if (ImGui::InputText("Tag", &entityTag))
						tag.Tag = entityTag;
				});

				DrawComponent<RelationshipComponent>("Relationship Component", selectedEntity, [&](auto& rel) {
					ImGui::Text("Parent: %llu", rel.ParentHandle);
					ImGui::Text("Children: %zu", rel.Children.size());
					ImGui::Indent(margin);
					for (const auto child : rel.Children)
					{
						Entity entity = m_Scene->TryGetEntityWithUUID(child);
						if (!entity)
							continue;

						ImGui::BulletText("%s", entity.GetTag().c_str());
					}	
					ImGui::Unindent(margin);
				});

				DrawComponent<NativeScriptComponent>("Native Script Component", selectedEntity, [](auto& script) {
					ImGui::Text("Bound Script: %s", script.ScriptName.c_str());
					if (script.Instance)
						script.Instance->OnImGuiHierarchyPanel();
				});

				DrawComponent<TransformComponent>("Transform Component", selectedEntity, [](auto& tc) {
					ImGui::DragFloat3("Translation", glm::value_ptr(tc.Translation), 1.0f);
					glm::vec3 rotation = glm::degrees(tc.Rotation);
					if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f))
						tc.Rotation = glm::radians(rotation);
					ImGui::DragFloat3("Scale",       glm::value_ptr(tc.Scale),       1.0f);
				});

				DrawComponent<CameraComponent>("Camera", selectedEntity, [&](auto& cam) {
					bool primary = false;
					if (m_Scene->GetPrimaryCameraEntity())
						primary = m_Scene->GetPrimaryCameraEntity().GetUUID() == selectedEntity.GetUUID();

					if (ImGui::Checkbox("Primary", &primary))
					{
						if (primary)
							m_Scene->SetPrimaryCamera(selectedEntity);
						else
							m_Scene->ResetPrimaryCamera();
					}

					auto& camera = cam.Camera;

					if (ImGui::Checkbox("Fixed Aspect Ratio", &cam.FixedAspectRatio))
					{
						if (!cam.FixedAspectRatio)
						{
							auto [width, height] = Application::Get().GetWindow()->GetSize();
							float aspectRatio = (float)width / (float)height;
							camera.SetAspectRatio(aspectRatio);
						}
					}
					ImGui::Checkbox("Use Transform", &cam.UseTransformComponent);

					if (cam.FixedAspectRatio)
					{
						float aspectRatio = camera.GetAspectRatio();
						if (ImGui::DragFloat("Aspect Ratio", &aspectRatio, 0.05f))
							camera.SetAspectRatio(aspectRatio);
					}

					float fov = glm::degrees(camera.GetFov());
					if (ImGui::DragFloat("Fov", &fov, 0.5f))
						camera.SetFov(glm::radians(fov));

					float nearClip = camera.GetNearClip();
					if (ImGui::DragFloat("Near clip", &nearClip, 0.1f))
						camera.SetNearClip(nearClip);

					float farClip = camera.GetFarClip();
					if (ImGui::DragFloat("Far clip", &farClip, 1.0f))
						camera.SetFarClip(farClip);

					glm::vec3 upDirection      = camera.GetUpDirection();
					glm::vec3 rightDirection   = camera.GetRightDirection();
					glm::vec3 forwardDirection = camera.GetForwardDirection();
					ImGui::Text("Up:      %f, %f, %f", upDirection.x,      upDirection.y,      upDirection.z);
					ImGui::Text("Right:   %f, %f, %f", rightDirection.x,   rightDirection.y,   rightDirection.z);
					ImGui::Text("Forward: %f, %f, %f", forwardDirection.x, forwardDirection.y, forwardDirection.z);
				});

				DrawComponent<SpriteRendererComponent>("Sprite Renderer", selectedEntity, [](auto& sprite) {
					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
					ImGui::DragFloat("Tiling Factor", &sprite.TilingFactor, 0.1f, 0.1f, 10.0f);
					ImGui::DragFloat2("UV Start", glm::value_ptr(sprite.UVStart), 0.01f);
					ImGui::DragFloat2("UV End", glm::value_ptr(sprite.UVEnd), 0.01f);

					if (sprite._Texture && sprite._Texture->IsValid())
					{
						ImVec2 size = { ImGui::GetContentRegionAvail().x , (float)sprite._Texture->GetHeight() * ImGui::GetContentRegionAvail().x / (float)sprite._Texture->GetWidth() };

						ImGui::Image(
							(ImTextureID)(sprite._Texture->GetRendererID()),
							size,
							ImVec2{ 0, 1 },
							ImVec2{ 1, 0 }
						);
					}
					else
					{
						ImGui::Text("No texture loaded.");
					}
				});
			}
		}

		ImGui::End();
	}

	void GameLayer::OnApplicationEvent(ApplicationEvent& e)
	{
		if (m_Scene)
			m_Scene->OnApplicationEvent(e);
	}

	void GameLayer::CreateWorld(const std::string& name)
	{
		if (name.empty())
		{
			KC_CORE_WARN("Cannot create world with empty name.");
			return;
		}

		std::filesystem::path worldPath = m_Config.Game.WorldsDir / std::filesystem::path(name);

		if (std::filesystem::exists(worldPath))
		{
			KC_CORE_WARN("World already exists: {}", worldPath.string());
			return;
		}

		try
		{
			if (!std::filesystem::create_directories(worldPath))
			{
				KC_CORE_ERROR("Failed to create world: {}", worldPath.string());
				return;
			}

			LoadWorld(name);
		}
		catch (const std::exception& e)
		{
			KC_CORE_ERROR("Exception while creating world: {}", e.what());
		}
	}

	void GameLayer::LoadWorld(const std::string& name)
	{
		if (!name.empty())
		{
			std::filesystem::path worldPath = m_Config.Game.WorldsDir / std::filesystem::path(name);
			if (std::filesystem::exists(worldPath) && std::filesystem::is_directory(worldPath))
			{
				m_Scene = CreateRef<Scene>(name);
				m_Scene->SetRenderer(m_Renderer);
				m_Scene->SetConfig(m_Config);
			}
			else
			{
				KC_CORE_ERROR("World folder does not exist: {}", worldPath.string());
			}
		}
	}

	void GameLayer::DeleteWorld(const std::string& name)
	{
		if (name.empty())
		{
			KC_CORE_WARN("Cannot delete world with empty name.");
			return;
		}

		std::filesystem::path worldPath = m_Config.Game.WorldsDir / std::filesystem::path(name);

		if (!std::filesystem::exists(worldPath))
		{
			KC_CORE_WARN("World does not exist: {}", worldPath.string());
			return;
		}

		try
		{
			std::filesystem::remove_all(worldPath);
		}
		catch (const std::exception& e)
		{
			KC_CORE_ERROR("Exception while deleting world '{}': {}", name, e.what());
		}
	}

	void GameLayer::DrawEntityNode(Entity entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

		if (entity.GetChildren().empty())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (entity.GetUUID() == m_HierarchyPanelSelectedEntity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entity.GetUUID(), flags, "%s", entity.GetTag().c_str());
		
		if (ImGui::IsItemClicked())
			m_HierarchyPanelSelectedEntity = entity.GetUUID();

		if (opened)
		{
			for (UUID child : entity.GetChildren())
			{
				Entity entity = m_Scene->TryGetEntityWithUUID(child);
				if (!entity)
					continue;

				DrawEntityNode(entity);
			}

			ImGui::TreePop();
		}
		
	}
}
