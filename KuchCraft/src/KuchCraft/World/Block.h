#pragma once

#include <stdint.h>
#include <string>

#include "Core/Base.h"
#include "KuchCraft/World/WorldCore.h"

namespace KuchCraft {

	constexpr uint32_t block_shift_id     = 0;
	constexpr uint32_t block_shift_state  = block_shift_id    + block_bits_for_id;
	constexpr uint32_t block_shift_flags  = block_shift_state + block_bits_for_state;
	constexpr uint32_t block_shift_custom = block_shift_flags + block_bits_for_flags;
	
	constexpr uint32_t block_mask_id     = BIT(block_bits_for_id)     - 1;
	constexpr uint32_t block_mask_state  = BIT(block_bits_for_state)  - 1;
	constexpr uint32_t block_mask_flags  = BIT(block_bits_for_flags)  - 1;
	constexpr uint32_t block_mask_custom = BIT(block_bits_for_custom) - 1;

	struct Block
	{
		uint32_t Raw = 0;

		ItemID  GetId()     const { return (Raw >> block_shift_id    ) & block_mask_id;     }
		uint8_t GetState()  const { return (Raw >> block_shift_state ) & block_mask_state;  }
		uint8_t GetFlags()  const { return (Raw >> block_shift_flags ) & block_mask_flags;  }
		uint8_t GetCustom() const { return (Raw >> block_shift_custom) & block_mask_custom; }

		void SetId    (ItemID   id    ) { Raw = (Raw & ~(block_mask_id     << block_shift_id))     | ((id     & block_mask_id)     << block_shift_id);     }
		void SetState (uint8_t  state ) { Raw = (Raw & ~(block_mask_state  << block_shift_state))  | ((state  & block_mask_state)  << block_shift_state);  }
		void SetFlags (uint8_t  flags ) { Raw = (Raw & ~(block_mask_flags  << block_shift_flags))  | ((flags  & block_mask_flags)  << block_shift_flags);  }
		void SetCustom(uint8_t  custom) { Raw = (Raw & ~(block_mask_custom << block_shift_custom)) | ((custom & block_mask_custom) << block_shift_custom); }

		void Set(ItemID id, uint8_t state = 0, uint8_t flags = 0, uint8_t custom = 0)
		{
			Raw =
				((id     & block_mask_id)     << block_shift_id)    |
				((state  & block_mask_state)  << block_shift_state) |
				((flags  & block_mask_flags)  << block_shift_flags) |
				((custom & block_mask_custom) << block_shift_custom);
		}

		bool IsAir() const { return GetId() == block_type_air; }
	};

	enum class BlockGeometryType : uint8_t
	{
		Cube = 0,
		Slab,
		Cross,
		Plane
	};

	enum class BlockFace : uint8_t {
		Front = 0,
		Left,
		Back,
		Right,
		Top,
		Bottom,
		Side, 
		All 
	};

	struct BlockData
	{
		BlockGeometryType GeometryType = BlockGeometryType::Cube;
		std::unordered_map<BlockFace, std::filesystem::path> Textures;

		float BreakingTime = 1.0f;
		float Weight       = 1.0f;
		float Friction     = 0.5f;

		bool    EmitsLight = false;
		uint8_t LightLevel = 0;

		bool Transparent   = false;	
		bool IsSolid       = true;
		bool IsOpaque      = true;
		bool HasCollision  = true;
		bool IsFluid       = false;
		bool IsReplaceable = false;
	};
}