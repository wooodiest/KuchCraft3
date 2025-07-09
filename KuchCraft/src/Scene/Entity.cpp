#include "kcpch.h"
#include "Scene/Entity.h"

namespace KuchCraft {

	bool Entity::IsValid() const
	{
		return (m_EntityHandle != entt::null) && m_Scene && m_Scene->m_Registry.valid(m_EntityHandle);
	}

	UUID Entity::GetSceneUUID() const
	{
		return m_Scene->GetUUID();
	}

	Entity Entity::GetParent() const
	{
		return m_Scene->TryGetEntityWithUUID(GetParentUUID());
	}

	bool Entity::IsAncestorOf(Entity entity) const
	{
		const auto& children = GetChildren();
		
		if (children.empty())
			return false;
		
		for (UUID child : children)
		{
			if (child == entity.GetUUID())
				return true;
		}
		
		for (UUID child : children)
		{
			if (m_Scene->GetEntityWithUUID(child).IsAncestorOf(entity))
				return true;
		}
		
		return false;
	}

}
