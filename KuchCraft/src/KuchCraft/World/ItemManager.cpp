#include "kcpch.h"
#include "KuchCraft/World/ItemManager.h"

namespace KuchCraft {

	ItemManager::ItemManager(const Config& config)
		: m_Config(config)
	{

	}

	ItemManager::~ItemManager()
	{
	}

	bool ItemManager::SetDataPack(const std::string& dataPackName)
	{
		const std::filesystem::path dataPackPath = m_Config.Game.DataPacksDir + dataPackName;
		if (!std::filesystem::exists(dataPackPath))
		{
			KC_CORE_ERROR("Data pack does not exist: {}", dataPackPath.string());
			return false;
		}

		const std::filesystem::path itemsDir = dataPackPath / "items";
		if (!std::filesystem::exists(itemsDir))
		{
			KC_CORE_ERROR("Items directory does not exist in data pack: {}", itemsDir.string());
			return false;
		}

		m_DataPackConfig.Name = dataPackName;

		ItemID currentID = block_type_air + 1;
		for (const auto& entry : std::filesystem::directory_iterator(itemsDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				std::ifstream file(entry.path());
				if (!file.is_open())
				{
					KC_CORE_ERROR("Failed to open item file: {}", entry.path().string());
					continue;
				}

				nlohmann::json itemJson;
				try
				{
					file >> itemJson;
					file.close();
				}
				catch (const std::exception& e)
				{
					KC_CORE_ERROR("Failed to parse JSON from file {}: {}", entry.path().string(), e.what());
					continue;
				}

				ItemData item = ParseItemJson(itemJson);
				if (item.Name.empty())
				{
					KC_CORE_ERROR("Item name is empty in file: {}", entry.path().string());
					continue;
				}

				m_ItemsData[currentID] = item;
				m_NameToID[item.Name]  = currentID;
				if (item.Block.has_value())
					m_BlocksData[currentID] = item.Block.value();

				currentID++;
			}
		}

		return true;
	}

	/// const ItemData& ItemManager::GetItemData(ItemID id) const
	/// {
	/// 	// TODO: insert return statement here
	/// }
	/// 
	/// const BlockData& ItemManager::GetBlockData(ItemID id) const
	/// {
	/// 	// TODO: insert return statement here
	/// }
	/// 
	/// const ItemData& ItemManager::GetItemData(const std::string_view& name) const
	/// {
	/// 	// TODO: insert return statement here
	/// }
	/// 
	/// const BlockData& ItemManager::GetBlockData(const std::string_view& name) const
	/// {
	/// 	// TODO: insert return statement here
	/// }

	ItemData ItemManager::ParseItemJson(const nlohmann::json& itemJson)
	{
		ItemData itemData;

		if (itemJson.contains("Name"))
			itemData.Name = itemJson["Name"].get<std::string>();
		if (itemJson.contains("DisplayName"))
			itemData.DisplayName = itemJson["DisplayName"].get<std::string>();
		if (itemJson.contains("Description"))
			itemData.Description = itemJson["Description"].get<std::string>();
		if (itemJson.contains("MaxStackSize"))
			itemData.MaxStackSize = itemJson["MaxStackSize"].get<uint8_t>();

		if (itemJson.contains("Block"))
		{
			nlohmann::json blockJson = itemJson["Block"];
			BlockData blockData;

			if (blockJson.contains("GeometryType"))
				blockData.GeometryType = FromString<BlockGeometryType>(blockJson["GeometryType"].get<std::string>()).value_or(BlockGeometryType::Cube);
			if (blockJson.contains("BreakingTime"))
				blockData.BreakingTime = blockJson["BreakingTime"].get<float>();
			if (blockJson.contains("Weight"))
				blockData.Weight = blockJson["Weight"].get<float>();
			if (blockJson.contains("Friction"))
				blockData.Friction = blockJson["Friction"].get<float>();
			if (blockJson.contains("EmitsLight"))
				blockData.EmitsLight = blockJson["EmitsLight"].get<bool>();
			if (blockJson.contains("LightLevel"))
				blockData.LightLevel = blockJson["LightLevel"].get<uint8_t>();
			if (blockJson.contains("Transparent"))
				blockData.Transparent = blockJson["Transparent"].get<bool>();
			if (blockJson.contains("IsSolid"))
				blockData.IsSolid = blockJson["IsSolid"].get<bool>();
			if (blockJson.contains("IsOpaque"))
				blockData.IsOpaque = blockJson["IsOpaque"].get<bool>();
			if (blockJson.contains("HasCollision"))
				blockData.HasCollision = blockJson["HasCollision"].get<bool>();
			if (blockJson.contains("IsFluid"))
				blockData.IsFluid = blockJson["IsFluid"].get<bool>();
			if (blockJson.contains("IsReplaceable"))
				blockData.IsReplaceable = blockJson["IsReplaceable"].get<bool>();

			itemData.Block = blockData;
		}
		return itemData;
	}

}
