#include "kcpch.h"
#include "Graphics/KuchCraft/ChunkMesh.h"

#include "KuchCraft/World/Chunk.h"
#include "KuchCraft/World/World.h"

namespace KuchCraft {

	ChunkMesh::ChunkMesh(Chunk* chunk)
		: m_Chunk(chunk)
	{
		m_GlobalPosition = glm::vec3(m_Chunk->GetPosition());
	}

	ChunkMesh::~ChunkMesh()
	{
	}
	
	constexpr glm::ivec3 GetFaceOffset(BlockFace face) {
		switch (face) {
			case BlockFace::Right:  return { 1,  0,  0 };
			case BlockFace::Left:   return { -1, 0,  0 };
			case BlockFace::Bottom: return { 0, -1,  0 };
			case BlockFace::Top:    return { 0,  1,  0 };
			case BlockFace::Front:  return { 0,  0,  1 };
			case BlockFace::Back:   return { 0,  0, -1 };
			default:                return { 0,  0,  0 };
		};
	};

	void ChunkMesh::Build()
	{
		if (!m_Chunk)
			return;
		
		m_MeshData.clear();
		m_MeshData.reserve(block_count_per_chunk);

		Ref<ItemManager> itemManager = m_Chunk->m_World->GetItemManager();

		size_t sectionOffset = 0;
		for (size_t sectionIndex = 0; sectionIndex < m_Chunk->GetSections().size(); sectionIndex++)
		{
			const auto& section = m_Chunk->GetSection(sectionIndex);
			for (uint32_t y = 0; y < section_size_y; y++)
			{
				for (uint32_t z = 0; z < section_size_z; z++)
				{
					for (uint32_t x = 0; x < section_size_x; x++)
					{
						glm::ivec3 inSectionPosition = { x, y, z };
						glm::ivec3 inChunkPosition   = { inSectionPosition.x, inSectionPosition.y + sectionIndex * section_size_y, inSectionPosition.z};

						Block     block     = section.GetBlock(inSectionPosition);
						BlockData blockData = itemManager->GetBlockDataUnsafe(block.GetId());

						if (block.IsAir() /* check for other flags or states from block data*/)
							continue;
						
						for (uint32_t i = 0; i < block_face_count; i++)
						{
							BlockFace face = static_cast<BlockFace>(i);

							const auto neighbor = GetNeighbor(inChunkPosition, inSectionPosition, face);
							if (!neighbor.has_value())
								continue;

							Block     neighborBlock     = neighbor.value();
							BlockData neighborBlockData = itemManager->GetBlockDataUnsafe(neighborBlock.GetId());
							if (!neighborBlock.IsAir() /* check for other flags or states */)
								continue;

							uint16_t layer = itemManager->GetBlockTextureLayer(block.GetId());
							for (uint8_t vert = 0; vert < block_vertices_per_face; vert++)
							{
								BlockMesh mesh( inChunkPosition.x, inChunkPosition.y, inChunkPosition.z, layer, i, 0, vert );
								m_MeshData.push_back(mesh);
							}
						}

					}
				}
			}
		}
	}

	std::optional<Block> ChunkMesh::GetNeighbor(const glm::ivec3& inChunkPosition, const glm::ivec3& inSectionPosition, BlockFace face)
	{
		glm::ivec3 neighborPos = inChunkPosition + GetFaceOffset(face);
		switch (face)
		{
			case BlockFace::Right:
			{
				if (neighborPos.x >= chunk_size_x) [[unlikely]]
				{
					Chunk* rightChunk = m_Chunk->GetRightNeighbor();
					if (!rightChunk || !rightChunk->IsBuilt())
						return std::nullopt;

					return rightChunk->GetBlock({ 0, neighborPos.y, neighborPos.z });
				}
				else [[likely]]
					return m_Chunk->GetBlock(neighborPos);
			}
			case BlockFace::Left:
			{
				if (neighborPos.x < 0) [[unlikely]]
				{
					Chunk* leftChunk = m_Chunk->GetLeftNeighbor();
					if (!leftChunk || !leftChunk->IsBuilt())
						return std::nullopt;

					return leftChunk->GetBlock({ chunk_size_x - 1, neighborPos.y, neighborPos.z });
				}
				else [[likely]]
					return m_Chunk->GetBlock(neighborPos);
			}
			case BlockFace::Front:
			{
				if (neighborPos.z >= chunk_size_z) [[unlikely]]
				{
					Chunk* frontChunk = m_Chunk->GetFrontNeighbor();
					if (!frontChunk || !frontChunk->IsBuilt())
						return std::nullopt;

					return frontChunk->GetBlock({ neighborPos.x, neighborPos.y, 0 });
				}
				else [[likely]]
					return m_Chunk->GetBlock(neighborPos);
			}
			case BlockFace::Back:
			{
				if (neighborPos.z < 0) [[unlikely]]
				{
					Chunk* backChunk = m_Chunk->GetBackNeighbor();
					if (!backChunk || !backChunk->IsBuilt())
						return std::nullopt;

					return backChunk->GetBlock({ neighborPos.x, neighborPos.y, chunk_size_z - 1 });
				}
				else [[likely]]
					return m_Chunk->GetBlock(neighborPos);
			}
			case BlockFace::Bottom:
			{
				if (neighborPos.y < 0) [[unlikely]]
				{
					/// Player should never be able to see this face, bedrock is always at y = 0
					return std::nullopt;
				}
				else [[likely]]
					return m_Chunk->GetBlock(neighborPos);
			}
			case BlockFace::Top:
			{
				if (neighborPos.y >= chunk_size_y) [[unlikely]]
				{
					/// We treat the top face as air, so we return an empty block
					return Block();
				}
				else [[likely]]
					return m_Chunk->GetBlock(neighborPos);
			}
		}

		return std::nullopt;
	}


} 