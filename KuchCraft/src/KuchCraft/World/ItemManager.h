#pragma once

#include "KuchCraft/World/Item.h"
#include "KuchCraft/World/Block.h"
#include "KuchCraft/World/WorldCore.h"

#include "Core/Config.h"

#include "Graphics/Core/Texture.h"

namespace KuchCraft {

	class ItemManager
	{
	public:
		ItemManager(const Config& config);
		~ItemManager();

		bool SetDataPack(const std::string& dataPackName);

		const ItemData* GetItemData(ItemID id) const;
		const ItemData* GetItemData(const std::string& name) const;
		const ItemData& GetItemDataUnsafe(ItemID id) const { return m_ItemsData.at(id); };
		const ItemData& GetItemDataUnsafe(const std::string& name) const { return m_ItemsData.at(m_NameToID.at(name)); };
		
		const BlockData* GetBlockData(ItemID id) const;
		const BlockData* GetBlockData(const std::string& name) const;
		const BlockData& GetBlockDataUnsafe(ItemID id) const { return GetItemDataUnsafe(id).Block.value(); };
		const BlockData& GetBlockDataUnsafe(const std::string& name) const { return GetItemDataUnsafe(name).Block.value();};

		int GetBlockTextureLayer(ItemID id) const
		{
			auto it = m_BlockTextureLayers.find(id);
			if (it != m_BlockTextureLayers.end())
				return it->second;
			return -1;
		}

		const auto& GetDataPackName() const { return m_DataPackConfig.Name; }
		const auto& GetDataPackPath() const { return m_DataPackPath; }
		const auto& GetItemsData()    const { return m_ItemsData;  }
		const auto& GetBlocksData()   const { return m_BlocksData; }
		const auto& GetNameToID()     const { return m_NameToID;   }

		const auto& GetItemTexture()  const { return m_ItemTexture; }
		const auto& GetBlockTexture() const { return m_BlockTexture; }
		const auto& GetBlockTextureLayers() const { return m_BlockTextureLayers; }

	private:
		void LoadConfig();
		void LoadItems();
		ItemData ParseItemJson(const nlohmann::json& itemJson);
		void LoadItemTextures();
		void LoadBlockTextures();

	private:
		Config m_Config;
		std::filesystem::path m_DataPackPath;
		std::filesystem::path m_ItemsDirPath;

		struct DataPackConfig
		{
			std::string Name;
			int TextureSize = 16;
		} m_DataPackConfig;

		std::map<ItemID, ItemData> m_ItemsData;
		std::map<ItemID, ItemData> m_BlocksData;

		std::map<std::string, ItemID>  m_NameToID;

		Ref<Texture2DArray> m_ItemTexture;
		Ref<Texture2DArray> m_BlockTexture;
		std::map<ItemID, int> m_BlockTextureLayers;
	};

}