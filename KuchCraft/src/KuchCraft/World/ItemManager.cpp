#include "kcpch.h"
#include "KuchCraft/World/ItemManager.h"

#include "Graphics/Core/GraphicsUtils.h"

#include <stb_image.h>

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

		m_DataPackPath = dataPackPath;
		m_ItemsDirPath = itemsDir;
		m_DataPackConfig.Name = dataPackName;

		LoadConfig();
		LoadItems();
		LoadItemTextures();

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

	void ItemManager::LoadConfig()
	{
		const std::filesystem::path filepath = m_Config.Game.DataPacksDir + m_DataPackConfig.Name + "/config.json";

		if (!std::filesystem::exists(filepath))
		{
			KC_CORE_WARN("File does not exist: {}", filepath.string());
			return;
		}

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			KC_CORE_ERROR("Failed to open file: {}", filepath.string());
			return;
		}

		nlohmann::json configJson;
		try
		{
			file >> configJson;
			file.close();
		}
		catch (std::exception& e)
		{
			KC_CORE_ERROR("Failed to parse JSON: {}", e.what());
			return;
		}

		if (configJson.contains("TextureSize"))
			m_DataPackConfig.TextureSize = configJson["TextureSize"].get<int>();
	}

	void ItemManager::LoadItems()
	{
		m_ItemsData[block_type_air] = ItemData{
			.Name         = "air",
			.DisplayName  = "Air",
			.Description  = "Invisible, non-solid block.",
			.TexturePath  = "", 
			.MaxStackSize = 0,
			.Block = BlockData{
				.GeometryType = BlockGeometryType::Cube,
				.BreakingTime  = 0.0f,
				.Weight        = 0.0f,
				.Friction      = 0.0f,
				.EmitsLight    = false,
				.LightLevel    = 0,
				.Transparent   = true,
				.IsSolid       = false,
				.IsOpaque      = false,
				.HasCollision  = false,
				.IsFluid       = false,
				.IsReplaceable = true
			}
		};

		ItemID currentID = block_type_air + 1;
		for (const auto& entry : std::filesystem::directory_iterator(m_ItemsDirPath))
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
				m_NameToID[item.Name] = currentID;
				if (item.Block.has_value())
					m_BlocksData[currentID] = item.Block.value();

				currentID++;
			}
		}
	}

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
		if (itemJson.contains("Texture"))
			itemData.TexturePath = itemJson["Texture"].get<std::filesystem::path>();

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

	void ItemManager::LoadItemTextures()
	{
		TextureSpecification spec;
		spec.Format = TextureFormat::RGBA;
		spec.Filter = TextureFilter::Nearest;
		spec.Wrap   = TextureWrap::Clamp;
		spec.Width  = m_DataPackConfig.TextureSize;
		spec.Height = m_DataPackConfig.TextureSize;
		m_ItemTexture = Texture2DArray::Create(spec, static_cast<int>(m_ItemsData.size()));
		m_ItemTexture->SetDebugName("Item Texture Array");

		for (const auto& [id, item] : m_ItemsData)
		{
			if (item.TexturePath.empty())
			{
				if (item.Block.has_value())
				{
					KC_TODO("Generate default texture as block for item with block data");

					/// Unsafe for different texture types
					std::vector<uint32_t> pixels(m_DataPackConfig.TextureSize * m_DataPackConfig.TextureSize, 0xffffffff);
					m_ItemTexture->SetLayerData(pixels.data(), pixels.size() * sizeof(uint32_t), id);

					continue;
				}
				else
				{
					KC_CORE_ERROR("Item {} has no texture path specified", item.Name);
				}
			}

			std::filesystem::path texturePath = m_DataPackPath / item.TexturePath;

			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);
			uint8_t* data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, Utils::GetTextureFormatChannelCount(spec.Format)); // RGBA forced

			if (!data || width != m_DataPackConfig.TextureSize || height != m_DataPackConfig.TextureSize)
			{
				KC_CORE_WARN("Failed to load or incorrect size for texture: {}", texturePath.string());

				/// Unsafe for different texture types
				std::vector<uint32_t> pixels(m_DataPackConfig.TextureSize * m_DataPackConfig.TextureSize, 0xffffffff);
				m_ItemTexture->SetLayerData(pixels.data(), pixels.size() * sizeof(uint32_t), id);

			}
			else
			{
				m_ItemTexture->SetLayerData(data, Utils::GetMemorySize(spec.Format, width, height), id);	
			}	

			stbi_image_free(data);
		}
	}

}
