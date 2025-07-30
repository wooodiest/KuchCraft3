#pragma once

#include "KuchCraft/World/Block.h"

namespace KuchCraft {

	class World;

	class ChunkSection
	{
	public:
		ChunkSection()  = default;
		~ChunkSection() = default;

		bool IsActive() const { return m_IsActive; }
		bool HasLight() const { return m_HasLight; }
		bool NeedsMeshUpdate() const { return m_NeedsMeshUpdate; }

		const std::array<Block, block_count_per_section>& GetBlocks() const { return m_Blocks; }

		Block GetBlock(const glm::ivec3& position) const
		{
			if (!m_IsActive) 
				return Block();

			return m_Blocks[Index(position)];
		}

		void SetBlock(const glm::ivec3& position, Block block)
		{
			if (!m_IsActive) 
				m_IsActive = true;

			m_Blocks[Index(position)] = block;
			m_NeedsMeshUpdate = true;
		}

	private:
		int Index(const glm::ivec3& position) const { return (position.y * section_size_z + position.z) * section_size_x + position.x; }

	private:
		std::array<Block, block_count_per_section> m_Blocks;

		bool m_IsActive = false;
		bool m_HasLight = false;
		bool m_NeedsMeshUpdate = false;
	};


	class Chunk
	{
	public:
		Chunk(const glm::ivec3& position, World* world);
		~Chunk();

		void OnTick(const Timestep ts);
		void OnUpdate(Timestep ts);

		void Build();
		void BuildMesh();

		Block GetBlock(const glm::ivec3& position) const;
		void  SetBlock(const glm::ivec3& position, Block block);

		inline static int ToSectionIndex(int y) { return y / section_size_y; }
		inline static glm::ivec3 ToLocalCoords(const glm::ivec3& position) {
			return { position.x & (section_size_x - 1), position.y & (section_size_y - 1), position.z & (section_size_z - 1) };
		}

	private:
		std::array<ChunkSection, sections_per_chunk> m_Sections;
		const glm::ivec3 m_Position = { 0.0f, 0.0f, 0.0f };
		World* m_World = nullptr;
	};

}