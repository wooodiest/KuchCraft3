#pragma once

#include <entt/entt.hpp>

namespace KuchCraft {

	class Entity;

	class Scene
	{
		/// Scene
		Scene(const std::string& name = "UntitledScene");
		virtual ~Scene();

		virtual void OnUpdate(float dt);
		virtual void OnTick(const Timestep ts);
		virtual void OnRender();
		virtual void OnApplicationEvent(ApplicationEvent& e);

		UUID GetUUID() const { return m_SceneID; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		/// Entities
		Entity CreateEntity(const std::string& name = "");
		Entity CreateChildEntity(Entity parent, const std::string& name = "");
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "");

		void SubmitToDestroyEntity(Entity entity);
		void DestroyEntity(Entity entity, bool excludeChildren = false, bool first = true);
		void DestroyEntity(UUID entityID, bool excludeChildren = false, bool first = true);

		Entity DuplicateEntity(Entity entity);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		Entity GetEntityWithUUID(UUID id) const;
		Entity TryGetEntityWithUUID(UUID id) const;

		template<typename Fn>
		void SubmitPostUpdateFunc(Fn&& func)
		{
			m_PostUpdateQueue.emplace_back(func);
		}

		const auto& GetEntityMap() const { return m_EntityIDMap; }

	private:
		UUID m_SceneID;
		std::string m_Name;
		entt::registry m_Registry;

		std::unordered_map<UUID, Entity> m_EntityIDMap;
		std::vector<std::function<void()>> m_PostUpdateQueue;

		friend class Entity;
	};

}