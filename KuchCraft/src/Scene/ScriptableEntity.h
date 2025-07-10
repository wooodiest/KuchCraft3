#pragma once

#include "Core/UUID.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"

namespace KuchCraft {

	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() {}

		template <typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}

		template <typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Entity.AddComponent<T>(std::forward<Args>(args)...);
		}

		template <typename T>
		void RemoveComponent()
		{
			m_Entity.RemoveComponent<T>();
		}

		std::string GetName()
		{
			return m_Entity.HasComponent<TagComponent>() ? m_Entity.GetComponent<TagComponent>().Tag : "";
		}

		UUID GetUUID()
		{
			return m_Entity.HasComponent<IDComponent>() ? m_Entity.GetComponent<IDComponent>().ID : UUID{ 0 };
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}

		virtual void OnTick(const Timestep ts) {}
		virtual void OnPreUpdate(Timestep ts) {}
		virtual void OnUpdate(Timestep ts) {} 
		virtual void OnPostUpdate(Timestep ts) {}

		Scene* GetScene() { return m_Entity.m_Scene; }

	private:
		Entity m_Entity;

		friend class Scene;
	};

}
