#pragma once

#include "KuchCraft/World/Item.h"
#include "KuchCraft/World/Block.h"
#include "KuchCraft/World/WorldCore.h"

#include "Core/Config.h"

namespace KuchCraft {


	class ItemManager
	{
	public:
		ItemManager(const Config& config);
		~ItemManager();

		bool SetDataPack(const std::string& dataPackName);

		/// C++ 20 and expected???
		/// const ItemData&  GetItemData (ItemID id) const;
		/// const BlockData& GetBlockData(ItemID id) const;
		/// 
		/// const ItemData&  GetItemData (const std::string_view& name) const;
		/// const BlockData& GetBlockData(const std::string_view& name) const;

		const auto& GetDataPackName() const { return m_DataPackConfig.Name; }
		const auto& GetItemsData()    const { return m_ItemsData;  }
		const auto& GetBlocksData()   const { return m_BlocksData; }
		const auto& GetNameToID()     const { return m_NameToID;   }

	private:
		ItemData ParseItemJson(const nlohmann::json& itemJson);

	private:
		Config m_Config;

		struct DataPackConfig
		{
			std::string Name;
			int TextureSize = 16;
		} m_DataPackConfig;

		std::map<ItemID, ItemData>  m_ItemsData;
		std::map<ItemID, BlockData> m_BlocksData;

		std::map<std::string_view, ItemID>  m_NameToID;
	};

}