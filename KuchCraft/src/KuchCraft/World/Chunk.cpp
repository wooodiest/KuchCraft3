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
	}

	void Chunk::BuildMesh()
	{

	}

	Block Chunk::GetBlock(const glm::ivec3& position) const
	{
		int section = ToSectionIndex(position.y);
		if (section < 0 || section >= sections_per_chunk)
		{
			KC_ERROR("Chunk::GetBlock: Invalid section index: {}", section);
			return Block();
		}

		return m_Sections[section].GetBlock(ToLocalCoords(position));
	}

	void Chunk::SetBlock(const glm::ivec3& position, Block block)
	{
		int section = ToSectionIndex(position.y);
		if (section < 0 || section >= sections_per_chunk)
		{
			KC_ERROR("Chunk::SetBlock: Invalid section index: {}", section);
			return ;
		}

		return m_Sections[section].SetBlock(ToLocalCoords(position), block);
	}

}