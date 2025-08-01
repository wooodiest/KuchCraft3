#include "kcpch.h"
#include "KuchCraft/World/World.h"

#include "Scene/Scene.h"

namespace KuchCraft {

	World::World(Scene* scene)
		: m_Scene(scene)
	{
		m_AssetManager = m_Scene->GetAssetManager();
		m_ItemManager  = m_Scene->GetItemManager();
		m_Renderer     = m_Scene->GetRenderer();

		m_ExampleChunk = CreateRef<Chunk>(glm::ivec3(0, 0, 0), this);
		m_ExampleChunk->Build();
		m_ExampleChunk->BuildMesh();
	}

	World::~World()
	{
	}

	void World::OnUpdate(Timestep ts)
	{
	}

	void World::OnTick(const Timestep ts)
	{
	}

	void World::OnRender()
	{
		m_Renderer->DrawChunkMesh(m_ExampleChunk->GetMesh());
	}

	Block World::GetBlock(const glm::ivec3& pos) const
	{
		return Block(); 
	}

	void World::SetBlock(const glm::ivec3& pos, Block block)
	{
	}


}