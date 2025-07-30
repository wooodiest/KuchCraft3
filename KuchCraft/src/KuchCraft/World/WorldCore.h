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

	constexpr uint32_t chunk_size_x = 16;
	constexpr uint32_t chunk_size_y = 256;
	constexpr uint32_t chunk_size_z = 16;

	constexpr uint32_t section_size_x = chunk_size_x;
	constexpr uint32_t section_size_y = 16;
	constexpr uint32_t section_size_z = chunk_size_z;

	constexpr uint32_t sections_per_chunk = chunk_size_y / section_size_y;

	constexpr uint32_t block_count_per_chunk   = chunk_size_x   * chunk_size_y   * chunk_size_z;
	constexpr uint32_t block_count_per_section = section_size_x * section_size_y * section_size_z;

}