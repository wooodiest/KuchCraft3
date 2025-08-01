#include "kcpch.h"
#include "KuchCraft/GameLayer.h"

#include "Core/Application.h"
#include "Scene/Entity.h"
#include "KuchCraft/CameraController.h"

#include "Graphics/Core/GraphicsUtils.h"

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
	static void ImGui_DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
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
		switch (m_GameState)
		{
			case GameState::MainMenu:	  ImGui_DrawMainMenuUI();      break;
			case GameState::InGame:       ImGui_DrawGameUI();          break;
			case GameState::PauseMenu:    ImGui_DrawPauseMenuUI();     break;
		}

		if (!m_Scene)
			return;

		ImGui::Begin("Game Debug Tools");
		constexpr float margin = 6.0f;

		if (ImGui::CollapsingHeader("Scene##GameLayer"))
		{
			ImGui::SeparatorText("Entities List");

			constexpr float entities_list_height = 250.0f;
			ImGui::BeginChild("EntitiesList", ImVec2(0.0f, entities_list_height), true);

			const auto& enities = m_Scene->GetEntityMap();
			for (const auto& [uuid, entity] : enities)
			{
				if (!entity.GetParent())
					ImGui_DrawEntityNode(entity);
			}
			ImGui::EndChild();

			Entity selectedEntity = m_Scene->TryGetEntityWithUUID(m_HierarchyPanelSelectedEntity);
			if (selectedEntity)
			{
				ImGui::SeparatorText("Entity info");
				ImGui::Text("UUID: %llu", selectedEntity.GetUUID());
				ImGui::Text("Tag: %s", selectedEntity.GetTag());

				ImGui_DrawComponent<TagComponent>("Tag Component", selectedEntity, [](auto& tag) {
					std::string entityTag = tag.Tag;
					if (ImGui::InputText("Tag", &entityTag))
						tag.Tag = entityTag;
				});

				ImGui_DrawComponent<RelationshipComponent>("Relationship Component", selectedEntity, [&](auto& rel) {
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

				ImGui_DrawComponent<NativeScriptComponent>("Native Script Component", selectedEntity, [](auto& script) {
					ImGui::Text("Bound Script: %s", script.ScriptName.c_str());
					if (script.Instance)
						script.Instance->OnImGuiHierarchyPanel();
				});

				ImGui_DrawComponent<TransformComponent>("Transform Component", selectedEntity, [](auto& tc) {
					ImGui::DragFloat3("Translation", glm::value_ptr(tc.Translation), 1.0f);
					glm::vec3 rotation = glm::degrees(tc.Rotation);
					if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.0f))
						tc.Rotation = glm::radians(rotation);
					ImGui::DragFloat3("Scale",       glm::value_ptr(tc.Scale),       1.0f);
				});

				ImGui_DrawComponent<CameraComponent>("Camera Component", selectedEntity, [&](auto& cam) {
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

				ImGui_DrawComponent<SpriteRendererComponent>("Sprite Renderer", selectedEntity, [&](auto& sprite) {
					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
					ImGui::DragFloat("Tiling Factor", &sprite.TilingFactor, 0.1f, 0.1f, 10.0f);
					ImGui::DragFloat2("UV Start", glm::value_ptr(sprite.UVStart), 0.01f);
					ImGui::DragFloat2("UV End", glm::value_ptr(sprite.UVEnd), 0.01f);

					ImGui::Text("Asset: %s", m_Scene->GetAssetManager()->GetName(sprite.Asset).c_str());
					ImGui::Text("UUID: %llu", sprite.Asset.ID);
					if (sprite.Asset.IsValid())
					{
						auto texture = m_Scene->GetAssetManager()->GetTexture2D(sprite.Asset);
						if (texture && texture->IsValid())
						{
							ImVec2 size = { ImGui::GetContentRegionAvail().x , (float)texture->GetHeight() * ImGui::GetContentRegionAvail().x / (float)texture->GetWidth() };
							ImGui::Image(
								(ImTextureID)(uintptr_t)(texture->GetRendererID()),
								size,
								ImVec2{ 0, 1 },
								ImVec2{ 1, 0 }
							);
						}
						else
						{
							ImGui::Text("No texture loaded.");
						}
					}
					else
					{
						ImGui::Text("No asset assigned.");
					}
				});

				ImGui_DrawComponent<PlaneRendererComponent>("Plane Renderer", selectedEntity, [&](auto& plane) {
					ImGui::ColorEdit4("Color", glm::value_ptr(plane.Color));
					ImGui::DragFloat("Tiling Factor", &plane.TilingFactor, 0.1f, 0.1f, 10.0f);
					ImGui::DragFloat2("UV Start", glm::value_ptr(plane.UVStart), 0.01f);
					ImGui::DragFloat2("UV End", glm::value_ptr(plane.UVEnd), 0.01f);

					ImGui::Text("Asset: %s", m_Scene->GetAssetManager()->GetName(plane.Asset).c_str());
					ImGui::Text("UUID: %llu", plane.Asset.ID);
					if (plane.Asset.IsValid())
					{
						auto texture = m_Scene->GetAssetManager()->GetTexture2D(plane.Asset);
						if (texture && texture->IsValid())
						{
							ImVec2 size = { ImGui::GetContentRegionAvail().x , (float)texture->GetHeight() * ImGui::GetContentRegionAvail().x / (float)texture->GetWidth() };
							ImGui::Image(
								(ImTextureID)(uintptr_t)(texture->GetRendererID()),
								size,
								ImVec2{ 0, 1 },
								ImVec2{ 1, 0 }
							);
						}
						else
						{
							ImGui::Text("No texture loaded.");
						}
					}
					else
					{
						ImGui::Text("No asset assigned.");
					}
				});
			}
		}

		if (ImGui::CollapsingHeader("Item Manager##GameLayer"))
		{
			Ref<ItemManager> itemManager = m_Scene->GetItemManager();

			ImGui::Text("Data Pack: %s", itemManager->GetDataPackName().c_str());
			ImGui::SeparatorText("Items List");

			constexpr float items_list_height = 250.0f;
			ImGui::BeginChild("ItemsList", ImVec2(0.0f, items_list_height), true);

			m_PerviousItemID = m_SelectedItemID;

			const auto& items = itemManager->GetItemsData();
			for (const auto& [id, item] : items)
			{
				bool isSelected = (m_SelectedItemID == id);
				if (ImGui::Selectable(item.DisplayName.c_str(), isSelected))
					m_SelectedItemID = id;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndChild();

			ItemData selected = items.at(m_SelectedItemID);

			ImGui::SeparatorText("Item info");
			ImGui::Text("ID: %d", m_SelectedItemID);
			ImGui::Text("Name: %s", selected.Name.c_str());

			if (m_PerviousItemID != m_SelectedItemID)
			{
				{
					TextureSpecification spec;
					spec.Format = itemManager->GetItemTexture()->GetFormat();
					spec.Width  = itemManager->GetItemTexture()->GetWidth();
					spec.Height = itemManager->GetItemTexture()->GetHeight();
					spec.Filter = TextureFilter::Nearest;

					m_SelectedItemTexture = Texture2D::Create(spec);
					itemManager->GetItemTexture()->CopyTo(m_SelectedItemTexture, m_SelectedItemID);
				}
				{
					TextureSpecification spec;
					spec.Format = itemManager->GetBlockTexture()->GetFormat();
					spec.Width  = itemManager->GetBlockTexture()->GetWidth();
					spec.Height = itemManager->GetBlockTexture()->GetHeight();
					spec.Filter = TextureFilter::Nearest;

					m_SelectedBlockTexture = Texture2D::Create(spec);
					itemManager->GetBlockTexture()->CopyTo(m_SelectedBlockTexture, itemManager->GetBlockTextureLayers().at(m_SelectedItemID));
				}
				
			}

			ImGui::Text("Texture:");
			if (m_SelectedItemTexture)
			{
				ImVec2 size = { ImGui::GetContentRegionAvail().x * 0.2f , (float)m_SelectedItemTexture->GetHeight() * ImGui::GetContentRegionAvail().x / (float)m_SelectedItemTexture->GetWidth() * 0.2f } ;
				ImGui::Image(
					(ImTextureID)(uintptr_t)(m_SelectedItemTexture->GetRendererID()),
					size,
					ImVec2{ 0, 1 },
					ImVec2{ 1, 0 }
				);
			}

			ImGui::Text("Display Name: %s", selected.DisplayName.c_str());
			ImGui::Text("Description: %s", selected.Description.c_str());
			ImGui::Text("Max Stack Size: %d", selected.MaxStackSize);

			if (selected.Block.has_value())
			{
				ImGui::SeparatorText("Block Data");
				const BlockData& blockData = selected.Block.value();

				ImGui::Text("Texture (Front, Left, Back, Right, Bottom)");
				if (m_SelectedBlockTexture)
				{
					ImVec2 size = { ImGui::GetContentRegionAvail().x , (float)m_SelectedBlockTexture->GetHeight() * ImGui::GetContentRegionAvail().x / (float)m_SelectedBlockTexture->GetWidth() };
					ImGui::Image(
						(ImTextureID)(uintptr_t)(m_SelectedBlockTexture->GetRendererID()),
						size,
						ImVec2{ 0, 1 },
						ImVec2{ 1, 0 }
					);
				}

				ImGui::Text("Geometry Type: %s", std::string(ToString(blockData.GeometryType)));
				ImGui::Text("Breaking Time: %f", blockData.BreakingTime);
				ImGui::Text("Weight: %f", blockData.Weight);
				ImGui::Text("Friction: %f", blockData.Friction);

				ImGui::Text("Emits Light: %s", blockData.EmitsLight ? "True" : "False");
				ImGui::Text("Light Level: %d", blockData.LightLevel);

				ImGui::Text("Transparent: %s", blockData.Transparent ? "True" : "False");
				ImGui::Text("Is Solid: %s", blockData.IsSolid ? "True" : "False");
				ImGui::Text("Is Opaque: %s", blockData.IsOpaque ? "True" : "False");
				ImGui::Text("Has Collision: %s", blockData.HasCollision ? "True" : "False");
				ImGui::Text("Is Fluid: %s", blockData.IsFluid ? "True" : "False");
				ImGui::Text("Is Replaceable: %s", blockData.IsReplaceable ? "True" : "False");
			}
		}

		if (ImGui::CollapsingHeader("Asset Manager##GameLayer"))
		{
			Ref<AssetManager> assetManager = m_Scene->GetAssetManager();

			ImGui::SeparatorText("Assets List");

			constexpr float assets_list_height = 250.0f;
			ImGui::BeginChild("AssetsList", ImVec2(0.0f, assets_list_height), true);

			const auto& items = assetManager->GetAssetsNames();
			for (const auto& [id, name] : items)
			{
				bool isSelected = (m_SelectedItemHandle.ID == id);
				if (ImGui::Selectable(name.c_str(), isSelected))
					m_SelectedItemHandle.ID = id;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndChild();

			switch (assetManager->GetType(m_SelectedItemHandle))
			{
				case AssetType::Texture2D:
				{
					ImGui::SeparatorText("Texture2D Asset Info");
					auto texture = assetManager->GetTexture2D(m_SelectedItemHandle);
					if (texture && texture->IsValid())
					{
						const auto& spec = texture->GetSpecification();
						ImGui::Text("ID: %llu", m_SelectedItemHandle.ID);
						ImGui::Text("Name: %s", assetManager->GetName(m_SelectedItemHandle).c_str());
						ImGui::Text("Format: %s", std::string(ToString(spec.Format)).c_str());
						ImGui::Text("Size: %dx%d", spec.Width, spec.Height);
						ImGui::Text("Filter: %s", std::string(ToString(spec.Filter)).c_str());
						ImGui::Text("Wrap: %s", std::string(ToString(spec.Wrap)).c_str());
						ImGui::Text("Memory Size: %zu bytes", Utils::GetMemorySize(spec.Format, spec.Width, spec.Height));
						ImGui::Text("Renderer ID: %llu", texture->GetRendererID());
						ImVec2 size = { ImGui::GetContentRegionAvail().x , (float)texture->GetHeight() * ImGui::GetContentRegionAvail().x / (float)texture->GetWidth() };
						ImGui::Image(
							(ImTextureID)(uintptr_t)(texture->GetRendererID()),
							size,
							ImVec2{ 0, 1 },
							ImVec2{ 1, 0 }
						);
					}
					else
					{
						ImGui::Text("No texture loaded.");
					}
					break;
				} 
				default:
					ImGui::Text("No asset selected.");
			}
		}

		ImGui::End();
	}

	void GameLayer::OnApplicationEvent(ApplicationEvent& e)
	{
		ApplicationEventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(KC_BIND_EVENT_FN(GameLayer::OnKeyPressed));

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
				m_GameState = GameState::InGame;

				m_Scene = CreateRef<Scene>(m_Config, m_Renderer, name);
				m_Scene->Load();
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

	bool GameLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		switch (e.GetKeyCode())
		{
			case Key::Escape: {
				if (m_Scene)
					m_GameState = m_GameState == GameState::PauseMenu ? GameState::InGame : GameState::PauseMenu;
			}
		}

		return false;
	}

	void GameLayer::ImGui_DrawEntityNode(Entity entity)
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

				ImGui_DrawEntityNode(entity);
			}

			ImGui::TreePop();
		}
		
	}

	void GameLayer::ImGui_DrawMainMenuUI()
	{
		ImGui::Begin("Main Menu");

		if (ImGui::Button("Close", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			Application::Get().Close();

		std::vector<std::string> worldNames;
		for (const auto& entry : std::filesystem::directory_iterator(m_Config.Game.WorldsDir))
		{
			if (entry.is_directory())
				worldNames.push_back(entry.path().filename().string());
		}

		ImGui::SeparatorText("Create new world");
		static const char* default_world_name = "New World";
		static std::string newWorldName       = default_world_name;
		ImGui::InputText("New world name", &newWorldName);
		if (ImGui::Button("Create world", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			CreateWorld(m_Config.Game.WorldsDir + newWorldName);
			newWorldName = default_world_name;
		}

		ImGui::SeparatorText("Worlds");
		for (const auto& worldName : worldNames)
		{
			if (ImGui::Button(worldName.c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 80.0f, 0.0f)))
				LoadWorld(worldName);

			ImGui::SameLine();
			if (ImGui::Button(std::string("Delete##" + worldName).c_str(), ImVec2(70.0f, 0.0f)))
				DeleteWorld(worldName);
		}

		ImGui::End();
	}

	void GameLayer::ImGui_DrawGameUI()
	{
		
	}

	void GameLayer::ImGui_DrawPauseMenuUI()
	{
		ImGui::Begin("Game Menu");

		if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
			m_Scene->Save();

		if (ImGui::Button("Quit", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
		{
			m_GameState = GameState::MainMenu;
			m_Scene.reset();
		}

		ImGui::End();
	}

}
