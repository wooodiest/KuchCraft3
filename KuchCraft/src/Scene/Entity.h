#pragma once

#include <entt/entt.hpp>

#include "Scene/Scene.h"

namespace KuchCraft {

	class Entity 
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene)
			: m_EntityHandle(handle), m_Scene(scene) { }
		~Entity() {}

		bool IsValid() const;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			KC_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			KC_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		const T& GetComponent() const
		{
			KC_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		T* TryGetComponent()
		{
			KC_CORE_ASSERT_NO_MSG(IsValid());
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename T>
		const T* TryGetComponent() const
		{
			KC_CORE_ASSERT_NO_MSG(IsValid());
			return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
		}

		template<typename... T>
		bool HasComponent() const
		{
			KC_CORE_ASSERT_NO_MSG(IsValid());
			return m_Scene->m_Registry.all_of<T...>(m_EntityHandle);
		}

		template<typename...T>
		bool HasAny() const
		{
			KC_CORE_ASSERT_NO_MSG(IsValid());
			return m_Scene->m_Registry.any_of<T...>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			KC_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponentIfExists()
		{
			KC_CORE_ASSERT(IsValid());
			if (m_Scene->m_Registry.all_of<T>(m_EntityHandle))
				m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		UUID GetSceneUUID() const;

		const std::string&        GetName()      const { return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Tag : NoName; }
		const UUID                GetUUID()      const { return GetComponent<IDComponent>().ID; }

		TransformComponent&       GetTransform()       { return GetComponent<TransformComponent>(); }
		const TransformComponent& GetTransform() const { return GetComponent<TransformComponent>(); }

		std::vector<UUID>&       GetChildren()       { return GetComponent<RelationshipComponent>().Children; }
		const std::vector<UUID>& GetChildren() const { return GetComponent<RelationshipComponent>().Children; }

		const UUID GetParentUUID() const { return GetComponent<RelationshipComponent>().ParentHandle; }
		void SetParentUUID(UUID parent)  { GetComponent<RelationshipComponent>().ParentHandle = parent; }

		bool RemoveChild(Entity child)
		{
			UUID childId = child.GetUUID();
			std::vector<UUID>& children = GetChildren();
			auto it = std::find(children.begin(), children.end(), childId);
			if (it != children.end())
			{
				children.erase(it);
				return true;
			}

			return false;
		}

		Entity GetParent() const;

		void SetParent(Entity parent)
		{
			Entity currentParent = GetParent();
			if (currentParent == parent)
				return;

			// If changing parent, remove child from existing parent
			if (currentParent)
				currentParent.RemoveChild(*this);

			// Setting to null is okay
			SetParentUUID(parent.GetUUID());

			if (parent)
			{
				auto& parentChildren = parent.GetChildren();
				UUID uuid = GetUUID();
				if (std::find(parentChildren.begin(), parentChildren.end(), uuid) == parentChildren.end())
					parentChildren.emplace_back(GetUUID());
			}
		}

		bool IsAncestorOf(Entity entity) const;
		bool IsDescendantOf(Entity entity) const { return entity.IsAncestorOf(*this); }

		operator uint32_t ()    const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator bool() const { return IsValid(); };

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;

		inline static std::string NoName = "Unnamed";

		friend class Scene;
		friend class ScriptableEntity;
	};

}