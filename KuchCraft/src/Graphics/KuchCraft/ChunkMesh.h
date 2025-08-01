#pragma once

#include "KuchCraft/World/WorldCore.h"

#include "KuchCraft/World/Block.h"

namespace KuchCraft {

	class Chunk;

	constexpr uint32_t block_mesh_bits_for_layer = block_bits_for_id;
	constexpr uint32_t block_mesh_bits_for_position_x = std::bit_width(chunk_size_x - 1);
	constexpr uint32_t block_mesh_bits_for_position_y = std::bit_width(chunk_size_y - 1);
	constexpr uint32_t block_mesh_bits_for_position_z = std::bit_width(chunk_size_z - 1);
	constexpr uint32_t block_mesh_bits_for_rotation     = 2;
	constexpr uint32_t block_mesh_bits_for_face         = 3;
	constexpr uint32_t block_mesh_bits_for_vertex_index = 3;

	constexpr uint8_t  block_vertices_per_face = 4;
	constexpr uint8_t  block_indicies_per_face = 6;

	constexpr uint32_t block_mesh_total_bits = block_mesh_bits_for_vertex_index + block_mesh_bits_for_face +
		block_mesh_bits_for_rotation   + block_mesh_bits_for_position_z + block_mesh_bits_for_position_y +
		block_mesh_bits_for_position_x + block_mesh_bits_for_layer;

	static_assert(block_mesh_total_bits <= 64, "BlockMesh does not fit in 64 bits!");

	constexpr uint32_t block_mesh_shift_vertex_index = 0;
	constexpr uint32_t block_mesh_shift_face         = block_mesh_shift_vertex_index + block_mesh_bits_for_vertex_index;
	constexpr uint32_t block_mesh_shift_rotation     = block_mesh_shift_face         + block_mesh_bits_for_face;
	constexpr uint32_t block_mesh_shift_position_z   = block_mesh_shift_rotation     + block_mesh_bits_for_rotation;
	constexpr uint32_t block_mesh_shift_position_y   = block_mesh_shift_position_z   + block_mesh_bits_for_position_z;
	constexpr uint32_t block_mesh_shift_position_x   = block_mesh_shift_position_y   + block_mesh_bits_for_position_y;
	constexpr uint32_t block_mesh_shift_layer        = block_mesh_shift_position_x   + block_mesh_bits_for_position_x;

	constexpr uint64_t block_mesh_mask_vertex_index = BIT(block_mesh_bits_for_vertex_index) - 1;
	constexpr uint64_t block_mesh_mask_face         = BIT(block_mesh_bits_for_face)         - 1;
	constexpr uint64_t block_mesh_mask_rotation     = BIT(block_mesh_bits_for_rotation)     - 1;
	constexpr uint64_t block_mesh_mask_position_z   = BIT(block_mesh_bits_for_position_z)   - 1;
	constexpr uint64_t block_mesh_mask_position_y   = BIT(block_mesh_bits_for_position_y)   - 1;
	constexpr uint64_t block_mesh_mask_position_x   = BIT(block_mesh_bits_for_position_x)   - 1;
	constexpr uint64_t block_mesh_mask_layer        = BIT(block_mesh_bits_for_layer)        - 1;

	struct BlockMesh
	{
		BlockMesh() = default;
		BlockMesh(uint8_t x, uint8_t y, uint8_t z, uint16_t layer, uint8_t face, uint8_t rot, uint8_t vert)
		{
			Set(x, y, z, layer, face, rot, vert);
		}

		uint32_t LowerBits = 0;
		uint32_t UpperBits = 0;
	
		uint8_t  GetVertexIndex() const { return (GetRaw() >> block_mesh_shift_vertex_index) & block_mesh_mask_vertex_index; }
		uint8_t  GetFace()        const { return (GetRaw() >> block_mesh_shift_face)         & block_mesh_mask_face; }
		uint8_t  GetRotation()    const { return (GetRaw() >> block_mesh_shift_rotation)     & block_mesh_mask_rotation; }
		uint8_t  GetZ()           const { return (GetRaw() >> block_mesh_shift_position_z)   & block_mesh_mask_position_z; }
		uint8_t  GetY()           const { return (GetRaw() >> block_mesh_shift_position_y)   & block_mesh_mask_position_y; }
		uint8_t  GetX()           const { return (GetRaw() >> block_mesh_shift_position_x)   & block_mesh_mask_position_x; }
		uint16_t GetLayer()       const { return (GetRaw() >> block_mesh_shift_layer)        & block_mesh_mask_layer; }
	
		void SetVertexIndex(uint8_t v) { ModifyBits(v, block_mesh_shift_vertex_index, block_mesh_mask_vertex_index); }
		void SetFace(uint8_t f)        { ModifyBits(f, block_mesh_shift_face,         block_mesh_mask_face); }
		void SetRotation(uint8_t r)    { ModifyBits(r, block_mesh_shift_rotation,     block_mesh_mask_rotation); }
		void SetZ(uint8_t z)           { ModifyBits(z, block_mesh_shift_position_z,   block_mesh_mask_position_z); }
		void SetY(uint8_t y)           { ModifyBits(y, block_mesh_shift_position_y,   block_mesh_mask_position_y); }
		void SetX(uint8_t x)           { ModifyBits(x, block_mesh_shift_position_x,   block_mesh_mask_position_x); }
		void SetLayer(uint16_t l)      { ModifyBits(l, block_mesh_shift_layer,        block_mesh_mask_layer); }
	
		void Set(uint8_t x, uint8_t y, uint8_t z, uint16_t layer, uint8_t face, uint8_t rot, uint8_t vert)
		{
			uint64_t raw = 0;
			raw |= (uint64_t(vert)  & block_mesh_mask_vertex_index) << block_mesh_shift_vertex_index;
			raw |= (uint64_t(face)  & block_mesh_mask_face)         << block_mesh_shift_face;
			raw |= (uint64_t(rot)   & block_mesh_mask_rotation)     << block_mesh_shift_rotation;
			raw |= (uint64_t(z)     & block_mesh_mask_position_z)   << block_mesh_shift_position_z;
			raw |= (uint64_t(y)     & block_mesh_mask_position_y)   << block_mesh_shift_position_y;
			raw |= (uint64_t(x)     & block_mesh_mask_position_x)   << block_mesh_shift_position_x;
			raw |= (uint64_t(layer) & block_mesh_mask_layer)        << block_mesh_shift_layer;
			SetRaw(raw);
		}
	
	private:
		void ModifyBits(uint64_t value, uint32_t shift, uint64_t mask)
		{
			uint64_t raw = GetRaw();
			raw = (raw & ~(mask << shift)) | ((value & mask) << shift);
			SetRaw(raw);
		}

		uint64_t GetRaw() const
		{
			return (uint64_t(UpperBits) << 32) | LowerBits;
		}

		void SetRaw(uint64_t value) 
		{
			LowerBits = static_cast<uint32_t>(value & 0xFFFFFFFF);
			UpperBits = static_cast<uint32_t>(value >> 32);
		}
	};

	class ChunkMesh
	{
	public:
		ChunkMesh(Chunk* chunk);
		~ChunkMesh();

		void Build();

		const std::vector<BlockMesh>& GetMeshData() const { return m_MeshData; }

		const glm::vec3& GetGlobalPosition() const { return m_GlobalPosition; }

	private:
		std::optional<Block> GetNeighbor(const glm::ivec3& inChunkPosition, const glm::ivec3& inSectionPosition, BlockFace face);

	private:
		Chunk* m_Chunk = nullptr;
		glm::vec3 m_GlobalPosition = { 0.0f, 0.0f, 0.0f };	

		std::vector<BlockMesh> m_MeshData;
	};

}