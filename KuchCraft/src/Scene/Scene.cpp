#include "kcpch.h"
#include "Scene/Scene.h"

#include "Core/Application.h"

#include "Scene/Entity.h"
#include "Scene/ScriptableEntity.h"

namespace KuchCraft {

	Scene::Scene(const std::string& name)
	{
		m_Registry.on_construct<NativeScriptComponent>().connect<&Scene::OnNativeScriptComponentAdded>(this);
		m_Registry.on_destroy<NativeScriptComponent>().connect<&Scene::OnNativeScriptComponentRemoved>(this);
	}

	Scene::~Scene()
	{
		for (auto entity : GetAllEntitiesWith<IDComponent>())
			DestroyEntity({ entity, this }, true, false);

		m_Registry.clear();
		m_Registry.on_construct<NativeScriptComponent>().disconnect<&Scene::OnNativeScriptComponentAdded>(this);
		m_Registry.on_destroy<NativeScriptComponent>().disconnect<&Scene::OnNativeScriptComponentRemoved>(this);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		if (m_IsPaused)
			return;

		for (auto&& fn : m_PreUpdateQueue)
			fn();
		m_PreUpdateQueue.clear();

		m_Registry.view<NativeScriptComponent>().each([&](auto entity, auto& script) {
			script.Instance->OnPreUpdate(ts);
		});

		m_Registry.view<NativeScriptComponent>().each([&](auto entity, auto& script){
			script.Instance->OnUpdate(ts);
		});

		m_Registry.view<NativeScriptComponent>().each([&](auto entity, auto& script) {
			script.Instance->OnPostUpdate(ts);
		});

		for (auto&& fn : m_PostUpdateQueue)
			fn();
		m_PostUpdateQueue.clear();
	}

	void Scene::OnTick(const Timestep ts)
	{
		if (m_IsPaused)
			return;

	}

	void Scene::OnRender()
	{
		if (!m_Renderer)
			return;

		Entity  cameraEntity = GetPrimaryCameraEntity();
		Camera* mainCamera   = nullptr;
		if (cameraEntity)
		{
			auto& cameraComponent    = cameraEntity.GetComponent<CameraComponent>();
			auto& transformComponent = cameraEntity.GetComponent<TransformComponent>();

			if (cameraComponent.UseTransformComponent)
				cameraComponent.Camera.UpdateTransform(transformComponent.Translation, transformComponent.Rotation);

			mainCamera = &cameraComponent.Camera;
		}

		if (!mainCamera)
			return;

		m_Registry.view<TransformComponent, SpriteRendererComponent>().each([&](auto entity, auto& transformComponent, auto& spriteComponent) {	
			if (spriteComponent.Texture)
				m_Renderer->DrawQuad2D(transformComponent.GetTransform(), spriteComponent.Texture,
					spriteComponent.TilingFactor, spriteComponent.Color, spriteComponent.UVStart, spriteComponent.UVEnd);
			else
				m_Renderer->DrawQuad2D(transformComponent.GetTransform(), spriteComponent.Color);
		});
	}

	void Scene::OnApplicationEvent(ApplicationEvent& e)
	{
		ApplicationEventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(KC_BIND_EVENT_FN(Scene::OnWindowResize));
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateChildEntity({}, name);
	}

	Entity Scene::CreateChildEntity(Entity parent, const std::string& name)
	{
		Entity entity = Entity{ m_Registry.create(), this };

		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = UUID();

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		if (parent)
			entity.SetParent(parent);

		m_EntityIDMap[idComponent.ID] = entity;

		return entity;
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		Entity entity = Entity{ m_Registry.create(), this };

		auto& idComponent = entity.AddComponent<IDComponent>();
		idComponent.ID = uuid;

		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RelationshipComponent>();
		if (!name.empty())
			entity.AddComponent<TagComponent>(name);

		m_EntityIDMap[idComponent.ID] = entity;

		return entity;
	}

	void Scene::SubmitToDestroyEntity(Entity entity)
	{
		if (!entity)
			return;

		SubmitPostUpdateFunc([entity]() { entity.m_Scene->DestroyEntity(entity); });
	}

	void Scene::DestroyEntity(Entity entity, bool excludeChildren, bool first)
	{
		if (!entity)
			return;

		if (!excludeChildren)
		{
			for (size_t i = 0; i < entity.GetChildren().size(); i++)
			{
				auto childId = entity.GetChildren()[i];
				Entity child = GetEntityWithUUID(childId);
				DestroyEntity(child, excludeChildren, false);
			}
		}

		if (first)
		{
			if (auto parent = entity.GetParent(); parent)
				parent.RemoveChild(entity);
		}

		UUID id = entity.GetUUID();
		m_Registry.destroy(entity.m_EntityHandle);
		m_EntityIDMap.erase(id);
	}

	void Scene::DestroyEntity(UUID entityID, bool excludeChildren, bool first)
	{
		auto it = m_EntityIDMap.find(entityID);
		if (it == m_EntityIDMap.end())
			return;

		DestroyEntity(it->second, excludeChildren, first);
	}	

	Entity Scene::GetEntityWithUUID(UUID id) const
	{
		KC_CORE_ASSERT(m_EntityIDMap.find(id) != m_EntityIDMap.end(), "Invalid entity ID or entity doesn't exist in scene!");
		return m_EntityIDMap.at(id);
	}

	Entity Scene::TryGetEntityWithUUID(UUID id) const
	{
		if (const auto iter = m_EntityIDMap.find(id); iter != m_EntityIDMap.end())
			return iter->second;

		return Entity{};
	}

	Entity Scene::GetEntityByName(const std::string& name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}

		return {};
	}

	void Scene::SetPrimaryCamera(Entity entity)
	{
		if (!entity.HasComponent<CameraComponent>())
		{
			KC_CORE_ERROR("Trying to set primary camera as entity without CameraComponent");
			return;
		}

		m_PrimaryCameraEntity = entity;
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		if (m_PrimaryCameraEntity == entt::null)
			return Entity();

		return Entity(m_PrimaryCameraEntity, this);
	}

	Camera* Scene::GetPrimaryCamera()
	{
		Entity cameraEntity = GetPrimaryCameraEntity();
		if (cameraEntity)
			return &cameraEntity.GetComponent<CameraComponent>().Camera;

		return nullptr;
	}

	bool Scene::OnWindowResize(WindowResizeEvent& e)
	{
		auto [width, height] = Application::Get().GetWindow()->GetSize();
		float aspectRatio = (float)width / (float)height;

		m_Registry.view<CameraComponent>().each([&](auto entity, auto cameraComponent) {
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetAspectRatio(aspectRatio);
		});

		return false;
	}

	void Scene::OnNativeScriptComponentAdded(entt::registry& registry, entt::entity entity)
	{
		SubmitPreUpdateFunc([=, this]() {
			if (!m_Registry.valid(entity))
				return;

			auto& script = m_Registry.get<NativeScriptComponent>(entity);
			if (!script.InstantiateScript)
				return;

			script.Instance = script.InstantiateScript();
			script.Instance->m_Entity = Entity{ entity, this };
			script.Instance->OnCreate();
		});
	}

	void Scene::OnNativeScriptComponentRemoved(entt::registry& registry, entt::entity entity)
	{
		auto& script = registry.get<NativeScriptComponent>(entity);

		if (script.Instance)
		{
			script.Instance->OnDestroy();
			script.DestroyScript(&script);
			script.Instance = nullptr;
		}
	}

}
