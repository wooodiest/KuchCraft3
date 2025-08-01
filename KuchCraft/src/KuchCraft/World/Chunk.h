#pragma once

#include "KuchCraft/World/Block.h"
#include "Graphics/KuchCraft/ChunkMesh.h"

namespace KuchCraft {

	class World;

	class ChunkSection
	{
	public:
		ChunkSection()  = default;
		~ChunkSection() = default;

		bool HasLight() const { return m_HasLight; }
		bool NeedsMeshUpdate() const { return m_NeedsMeshUpdate; }

		const std::array<Block, block_count_per_section>& GetBlocks() const { return m_Blocks; }

		Block GetBlock(const glm::ivec3& position) const
		{
			return m_Blocks[Index(position)];
		}

		void SetBlock(const glm::ivec3& position, Block block)
		{
			m_Blocks[Index(position)] = block;
			m_NeedsMeshUpdate = true;
		}

		int Index(const glm::ivec3& position) const { return (position.y * section_size_z + position.z) * section_size_x + position.x; }

		inline bool IsInside(const glm::ivec3& pos) const
		{
			return pos.x >= 0 && pos.y >= 0 && pos.z >= 0 &&
				pos.x < section_size_x && pos.y < section_size_y && pos.z < section_size_z;
		}

	private:
		std::array<Block, block_count_per_section> m_Blocks;

		bool m_HasLight = false;
		bool m_NeedsMeshUpdate = false;

		friend class ChunkMesh;
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

		bool IsBuilt() const { return m_IsBuilt; }

		Block GetBlock(const glm::ivec3& position) const { return m_Sections[ToSectionIndex(position.y)].GetBlock(ToSectionCoords(position)); }
		Block GetBlockSafe(const glm::ivec3& position) const;
		void  SetBlock(const glm::ivec3& position, Block block);

		inline static int ToSectionIndex(int y) { return y / section_size_y; }
		inline static glm::ivec3 ToSectionCoords(const glm::ivec3& position) {
			return { position.x & (section_size_x - 1), position.y & (section_size_y - 1), position.z & (section_size_z - 1) };
		}

		Chunk* GetLeftNeighbor()  const { return nullptr; } // tmp
		Chunk* GetRightNeighbor() const { return nullptr; } // tmp
		Chunk* GetFrontNeighbor() const { return nullptr; } // tmp
		Chunk* GetBackNeighbor()  const { return nullptr; } // tmp

		const auto& GetSections() const { return m_Sections; }
		const auto& GetSection(size_t index) const { return m_Sections[index]; }
		const auto& GetSectionSafe(size_t index) const;

		const glm::ivec3& GetPosition() const { return m_Position; }

		Ref<ChunkMesh> GetMesh() const { return m_Mesh; }

	private:
		std::array<ChunkSection, sections_per_chunk> m_Sections;
		const glm::ivec3 m_Position = { 0.0f, 0.0f, 0.0f };
		World* m_World = nullptr;

		bool m_IsBuilt = false;

		Ref<ChunkMesh> m_Mesh;

		friend class ChunkMesh;
	};

}