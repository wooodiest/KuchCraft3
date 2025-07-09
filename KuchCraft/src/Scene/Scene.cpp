#include "kcpch.h"
#include "Scene/Scene.h"

namespace KuchCraft {

	Scene::Scene(const std::string& name)
	{

	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(float dt)
	{
	}

	void Scene::OnTick(const Timestep ts)
	{
	}

	void Scene::OnRender()
	{
	}

	void Scene::OnApplicationEvent(ApplicationEvent& e)
	{
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

}
