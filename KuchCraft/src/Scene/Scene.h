#pragma once

#include <entt/entt.hpp>

#include "Graphics/Renderer.h"

namespace KuchCraft {

	class Entity;

	class Scene
	{
	public:
		/// Scene
		Scene(const std::string& name = "UntitledScene");
		~Scene();

		void OnUpdate(Timestep ts);
		void OnTick(const Timestep ts);
		void OnRender();
		void OnApplicationEvent(ApplicationEvent& e);

		UUID GetUUID() const { return m_SceneID; }

		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		void Pause(bool value) { m_IsPaused = value; }

		void SetRenderer(const Ref<Renderer>& renderer) { m_Renderer = renderer; }

		/// Entities
		Entity CreateEntity(const std::string& name = "");
		Entity CreateChildEntity(Entity parent, const std::string& name = "");
		Entity CreateEntityWithID(UUID uuid, const std::string& name = "");

		void SubmitToDestroyEntity(Entity entity);
		void DestroyEntity(Entity entity, bool excludeChildren = false, bool first = true);
		void DestroyEntity(UUID entityID, bool excludeChildren = false, bool first = true);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		Entity GetEntityWithUUID(UUID id) const;
		Entity TryGetEntityWithUUID(UUID id) const;
		Entity GetEntityNyName(const std::string& name);

		template<typename Fn>
		void SubmitPreUpdateFunc(Fn&& func)
		{
			m_PreUpdateQueue.emplace_back(func);
		}

		template<typename Fn>
		void SubmitPostUpdateFunc(Fn&& func)
		{
			m_PostUpdateQueue.emplace_back(func);
		}

		const auto& GetEntityMap() const { return m_EntityIDMap; }

		void SetPrimaryCamera(Entity entity);
		Entity GetPrimaryCameraEntity();
		Camera* GetPrimaryCamera();

	private:
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		UUID m_SceneID;
		std::string m_Name;
		entt::registry m_Registry;
		bool m_IsPaused = false;

		Ref<Renderer> m_Renderer;

		std::unordered_map<UUID, Entity> m_EntityIDMap;
		std::vector<std::function<void()>> m_PostUpdateQueue;
		std::vector<std::function<void()>> m_PreUpdateQueue;

		entt::entity m_PrimaryCameraEntity = entt::null;

		friend class Entity;
	};

}