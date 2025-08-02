#include "kcpch.h"
#include "KuchCraft/World/World.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"

namespace KuchCraft {

	World::World(Scene* scene, Config m_Config)
		: m_Scene(scene), m_Config(m_Config)
	{
		m_AssetManager = m_Scene->GetAssetManager();
		m_ItemManager  = m_Scene->GetItemManager();
		m_Renderer     = m_Scene->GetRenderer();

		m_WorldGenerator = CreateRef<WorldGenerator>(m_Config);
	}

	World::~World()
	{
	}

	void World::OnUpdate(Timestep ts)
	{
		if (m_Scene)
		{
			Entity playerEntity = m_Scene->GetPlayerEntity();
			if (playerEntity)
			{
				if (playerEntity.HasComponent<TransformComponent>())
				{
					auto& transform = playerEntity.GetComponent<TransformComponent>();
					m_PlayerPosition = transform.Translation;
				}
			}
		}

		const float renderDistanceStep = (float)glm::sqrt(chunk_size_x * chunk_size_x + chunk_size_z * chunk_size_z);
		const float renderDistance     = m_Config.Renderer.RenderDistance * renderDistanceStep;

		/// Create chunks around the player position
		const float createDistanceStepX = (float)chunk_size_x;
		const float createDistanceStepZ = (float)chunk_size_z;
		const float createDistance     = renderDistance + renderDistanceStep;
		for (float dx = -createDistance; dx <= createDistance; dx += createDistanceStepX)
		{
			for (float dz = -createDistance; dz <= createDistance; dz += createDistanceStepZ)
			{
				glm::vec3 chunkPos = m_PlayerPosition + glm::vec3(dx, 0.0f, dz);
				if (!GetChunk(chunkPos))
					CreateChunk(chunkPos);
			}
		}

		/// Remove chunks that are too far away from the player position
		const float deleteDistance  = renderDistance * 2.0f;
		const float deleteDistance2 = deleteDistance * deleteDistance;
		for (auto it = m_Chunks.begin(); it != m_Chunks.end();)
		{
			float distance2 = glm::length2(glm::vec2(m_PlayerPosition.x, m_PlayerPosition.z) - glm::vec2(it->first));
			if (distance2 > deleteDistance2)
				it = m_Chunks.erase(it); 
			else
				++it; 
		}

		uint32_t maxPerFrame = 1;
		uint32_t count = 0;
		for (auto& [position, chunk] : m_Chunks)
		{
			if (!chunk->IsBuilt())
			{
				m_WorldGenerator->GenerateChunk(chunk);
				chunk->Build();
				chunk->BuildMesh();

				count++;
				if (count >= maxPerFrame)
					return; 
			}
		}
	}

	void World::OnTick(const Timestep ts)
	{
	}

	void World::OnRender()
	{
		for (auto& [position, chunk] : m_Chunks)
		{
			if (chunk->GetMesh() && !chunk->GetMesh()->IsEmpty())
				m_Renderer->DrawChunkMesh(chunk->GetMesh());
		}
	}

	Block World::GetBlock(const glm::ivec3& pos) const
	{
		return Block(); 
	}

	void World::SetBlock(const glm::ivec3& pos, Block block)
	{
	}

	Ref<Chunk> World::CreateChunk(const glm::vec3& pos)
	{
		glm::ivec3 chunkPos = GetChunkPosition(pos);
		auto chunk = CreateRef<Chunk>(chunkPos, this);
		m_Chunks[chunkPos] = chunk;

		return chunk;
	}


}