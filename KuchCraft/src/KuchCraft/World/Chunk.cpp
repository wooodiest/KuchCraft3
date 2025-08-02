#include "kcpch.h"
#include "KuchCraft/World/Chunk.h"

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

		for (uint32_t y = 0; y < chunk_size_y; y++)
		{
			for (uint32_t z = 0; z < chunk_size_z; z++)
			{
				for (uint32_t x = 0; x < chunk_size_x; x++)
				{
					if (y < 50)
					{
						Block block;

						if (x % 3 == 0)
							block.SetId(1);
						else if (x % 3 == 1)
							block.SetId(2);
						else if(x % 3 == 2)
							block.SetId(3);

						SetBlock({ x, y, z }, block);
					}
				}
			}
		}
		Block block;
		block.SetId(2);
		SetBlock({ 8, 51, 8 }, block);

		m_IsBuilt = true;
	}

	void Chunk::BuildMesh()
	{
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