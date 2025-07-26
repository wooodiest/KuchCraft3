#pragma once

#include <stdint.h>

namespace KuchCraft {

	using ItemID = uint16_t;

	constexpr uint32_t block_bits_for_id     = 12;
	constexpr uint32_t block_bits_for_state  = 8;
	constexpr uint32_t block_bits_for_flags  = 6;
	constexpr uint32_t block_bits_for_custom = 6;

	constexpr uint32_t block_face_count = 6;

	constexpr ItemID block_type_air = 0;

}