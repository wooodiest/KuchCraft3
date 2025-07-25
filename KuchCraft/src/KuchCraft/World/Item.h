#pragma once

#include <stdint.h>
#include <string>
#include <optional>

#include "KuchCraft/World/Block.h"

namespace KuchCraft {

	struct Item
	{
		ItemID   ID     = 0;
		uint16_t Count  = 1;
	};

	struct ItemData
	{
		std::string Name;
		std::string DisplayName;
		std::string Description;

		std::filesystem::path TexturePath;

		uint8_t MaxStackSize = 64;

		std::optional<BlockData> Block;

	};

}