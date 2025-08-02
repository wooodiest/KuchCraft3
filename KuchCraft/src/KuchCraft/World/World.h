#pragma once

#include "KuchCraft/World/Chunk.h"

#include "KuchCraft/World/ItemManager.h"
#include "Scene/AssetManager.h"

namespace KuchCraft {

	class Scene;
	class Renderer;

	class World
	{
	public:
		World(Scene* scene, Config config);
		~World();

		void OnUpdate(Timestep ts);
		void OnTick(const Timestep ts);
		void OnRender();

		Block GetBlock(const glm::ivec3& pos) const;
		void  SetBlock(const glm::ivec3& pos, Block block);

		Ref<Chunk> CreateChunk(const glm::vec3& pos);

		Ref<Chunk> GetChunk(const glm::vec3& pos)
		{
			auto it = m_Chunks.find(GetChunkPosition(pos));
			if (it != m_Chunks.end())
				return it->second;

			return nullptr;
		}

		Ref<Chunk> GetOrCreateChunk(const glm::vec3& pos)
		{
			auto chunk = GetChunk(pos);
			if (!chunk)
				chunk = CreateChunk(pos);

			return chunk;
		}

		Ref<ItemManager>  GetItemManager()  const { return m_ItemManager; }
		Ref<AssetManager> GetAssetManager() const { return m_AssetManager; }
		Ref<Renderer>     GetRenderer()     const { return m_Renderer; }

		static glm::ivec3 GetChunkPosition(const glm::vec3& pos) { return glm::ivec3(std::floor(pos.x / chunk_size_x) * chunk_size_x, 0.0f, std::floor(pos.z / chunk_size_z) * chunk_size_z); };

	private:
		Scene* m_Scene = nullptr;
		Config m_Config;
		Ref<Renderer>     m_Renderer;
		Ref<ItemManager>  m_ItemManager;
		Ref<AssetManager> m_AssetManager;

		std::unordered_map<glm::ivec3, Ref<Chunk>> m_Chunks;
	};

}