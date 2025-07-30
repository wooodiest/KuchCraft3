#pragma once

#include "KuchCraft/World/Chunk.h"

#include "KuchCraft/World/ItemManager.h"
#include "Scene/AssetManager.h"
#include "Graphics/Renderer.h"

namespace KuchCraft {

	class Scene;

	class World
	{
	public:
		World(Scene* scene);
		~World();

		void OnUpdate(Timestep ts);
		void OnTick(const Timestep ts);
		void OnRender();

		Block GetBlock(const glm::ivec3& pos) const;
		void  SetBlock(const glm::ivec3& pos, Block block);

	private:
		Scene* m_Scene = nullptr;
		Ref<Renderer>     m_Renderer;
		Ref<ItemManager>  m_ItemManager;
		Ref<AssetManager> m_AssetManager;

		std::unordered_map<glm::ivec2, Chunk> m_Chunks;
	};

}