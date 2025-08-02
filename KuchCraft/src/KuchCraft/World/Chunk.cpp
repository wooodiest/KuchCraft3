#include "kcpch.h"
#include "KuchCraft/World/Chunk.h"

#include "KuchCraft/World/WorldGenerator.h"

#include "KuchCraft/World/World.h"

namespace KuchCraft {

	Chunk::Chunk(const glm::ivec3& position, World* world)
		: m_Position(position), m_World(world)
	{

	}

	Chunk::~Chunk()
	{

	}

	void Chunk::OnTick(const Timestep ts)
	{

	}

	void Chunk::OnUpdate(Timestep ts)
	{

	}

	void Chunk::Build()
	{
		m_IsBuilt = true;
	}

	void Chunk::BuildMesh()
	{
		if (!m_IsBuilt)
			return;

		if (!m_Mesh)
			m_Mesh = CreateRef<ChunkMesh>(this);

		m_Mesh->Build();
	}

	Block Chunk::GetBlockSafe(const glm::ivec3& position) const
	{
		int section = ToSectionIndex(position.y);
		if (section < 0 || section >= sections_per_chunk)
		{
			KC_ERROR("Chunk::GetBlock: Invalid section index: {}", section);
			return Block();
		}

		return m_Sections[section].GetBlock(ToSectionCoords(position));
	}

	void Chunk::SetBlock(const glm::ivec3& position, Block block)
	{
		int section = ToSectionIndex(position.y);
		if (section < 0 || section >= sections_per_chunk)
		{
			KC_ERROR("Chunk::SetBlock: Invalid section index: {}", section);
			return ;
		}

		return m_Sections[section].SetBlock(ToSectionCoords(position), block);
	}

	Ref<Chunk> Chunk::GetLeftNeighbor() const
	{
		return m_World->GetChunk({ m_Position.x - chunk_size_x, m_Position.y, m_Position.z });	
	}

	Ref<Chunk> Chunk::GetRightNeighbor() const
	{
		return m_World->GetChunk({ m_Position.x + chunk_size_x, m_Position.y, m_Position.z });
		
	}

	Ref<Chunk> Chunk::GetFrontNeighbor() const
	{
		return m_World->GetChunk({ m_Position.x, m_Position.y, m_Position.z + chunk_size_z });
	}

	Ref<Chunk> Chunk::GetBackNeighbor() const
	{
		return m_World->GetChunk({ m_Position.x, m_Position.y, m_Position.z - chunk_size_z });
	}

	const auto& Chunk::GetSectionSafe(size_t index) const
	{
		if (index < 0 || index >= sections_per_chunk)
		{
			KC_ERROR("Chunk::GetSection: Invalid section index: {}", index);
			return m_Sections[0];
		}
		return m_Sections[index];
	}

}