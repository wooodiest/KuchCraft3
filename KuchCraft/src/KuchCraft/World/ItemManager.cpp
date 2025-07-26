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
		LoadBlockTextures();

		return true;
	}

	const ItemData* ItemManager::GetItemData(ItemID id) const
	{
		auto it = m_ItemsData.find(id);
		if (it == m_ItemsData.end())
			return nullptr;
		return &it->second;
	}

	const ItemData* ItemManager::GetItemData(const std::string& name) const
	{
		auto it = m_NameToID.find(name);
		if (it == m_NameToID.end())
			return nullptr;

		return GetItemData(it->second);
	}

	const BlockData* ItemManager::GetBlockData(ItemID id) const
	{
		const ItemData* item = GetItemData(id);
		if (!item || !item->Block.has_value())
			return nullptr;
		return &item->Block.value();
	}

	const BlockData* ItemManager::GetBlockData(const std::string& name) const
	{
		const ItemData* item = GetItemData(name);
		if (!item || !item->Block.has_value())
			return nullptr;
		return &item->Block.value();
	}

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

		m_BlocksData[block_type_air] = m_ItemsData[block_type_air];
		m_NameToID["air"] = block_type_air;

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
				m_NameToID[item.Name]  = currentID;
				if (item.Block.has_value())
					m_BlocksData[currentID] = item;

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

			if (blockJson.contains("Textures"))
			{
				const auto& texturesJson = blockJson["Textures"];
				for (auto it = texturesJson.begin(); it != texturesJson.end(); ++it)
				{
					std::string key = it.key();
					std::string path = it.value().get<std::string>();

					std::optional<BlockFace> face = FromString<BlockFace>(key);
					if (face.has_value())
						blockData.Textures[face.value()] = path;
					else
					{
						KC_CORE_WARN("Unknown block texture face '{}'", key);
					}
				}
			}

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

					size_t textureSize = Utils::GetMemorySize(spec.Format, m_DataPackConfig.TextureSize, m_DataPackConfig.TextureSize);
					uint8_t* data = new uint8_t[textureSize];
					std::memset(data, 0xff, textureSize);
					m_ItemTexture->SetLayerData(data, textureSize, id);
					delete[] data;

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
			uint8_t* data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, Utils::GetTextureFormatChannelCount(spec.Format));

			if (!data || width != m_DataPackConfig.TextureSize || height != m_DataPackConfig.TextureSize)
			{
				KC_CORE_WARN("Failed to load or incorrect size for texture: {}", texturePath.string());
				stbi_image_free(data);

				size_t textureSize = Utils::GetMemorySize(spec.Format, m_DataPackConfig.TextureSize, m_DataPackConfig.TextureSize);
				data = new uint8_t[textureSize];
				std::memset(data, 0xff, textureSize);
				m_ItemTexture->SetLayerData(data, textureSize, id);
				delete[] data;
			}
			else
			{
				m_ItemTexture->SetLayerData(data, Utils::GetMemorySize(spec.Format, width, height), id);
				stbi_image_free(data);
			}		
		}
	}

	void ItemManager::LoadBlockTextures()
	{
		TextureSpecification spec;
		spec.Format = TextureFormat::RGBA;
		spec.Filter = TextureFilter::Nearest;
		spec.Wrap   = TextureWrap::Clamp;
		spec.Width  = m_DataPackConfig.TextureSize * block_face_count;
		spec.Height = m_DataPackConfig.TextureSize;
		m_BlockTexture = Texture2DArray::Create(spec, static_cast<int>(m_BlocksData.size()));
		m_BlockTexture->SetDebugName("Blocks Texture Array");

		int layer = 0;
		for (const auto& [id, item] : m_BlocksData)
		{
			const BlockData& block = item.Block.value();

			std::array<std::string, block_face_count> texturePaths;
			auto getPath = [&](BlockFace face) -> std::filesystem::path {
				auto it = block.Textures.find(face);
				if (it != block.Textures.end())
					return it->second;
				return "";
			};

			std::string all  = getPath(BlockFace::All).string();
			std::string side = getPath(BlockFace::Side).string();

			texturePaths[0] = getPath(BlockFace::Front).string();   /// Front
			texturePaths[1] = getPath(BlockFace::Left).string();    /// Left
			texturePaths[2] = getPath(BlockFace::Back).string();    /// Back
			texturePaths[3] = getPath(BlockFace::Right).string();   /// Right
			texturePaths[4] = getPath(BlockFace::Top).string();     /// Top
			texturePaths[5] = getPath(BlockFace::Bottom).string();  /// Bottom

			for (int i = 0; i < block_face_count; i++)
			{
				if (texturePaths[i].empty())
					texturePaths[i] = (i == 4 || i == 5) ? all : (!side.empty() ? side : all);
			}

			size_t textureSize = Utils::GetMemorySize(spec.Format, m_DataPackConfig.TextureSize * block_face_count, m_DataPackConfig.TextureSize);
			uint8_t* mergedData = new uint8_t[textureSize];
			std::memset(mergedData, 0xff, textureSize);

			for (int i = 0; i < block_face_count; i++)
			{
				std::filesystem::path path = m_DataPackPath / texturePaths[i];
				if (!std::filesystem::exists(path))
				{
					KC_CORE_WARN("Missing texture for block '{}': {}", item.Name, path.string());
					continue;
				}

				int width, height, channels, desiredChannels = Utils::GetTextureFormatChannelCount(spec.Format);
				stbi_set_flip_vertically_on_load(1);
				uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, desiredChannels);

				if (!data || width != m_DataPackConfig.TextureSize || height != m_DataPackConfig.TextureSize)
				{
					KC_CORE_WARN("Failed to load or incorrect size for texture: {}", path.string());
					stbi_image_free(data);
					continue;
				}

				for (int y = 0; y < m_DataPackConfig.TextureSize; y++)
				{
					std::memcpy(
						mergedData + (y * spec.Width * desiredChannels) + (i * m_DataPackConfig.TextureSize * desiredChannels),
						data + (y * m_DataPackConfig.TextureSize * desiredChannels),
						m_DataPackConfig.TextureSize * desiredChannels);
				}
				
				stbi_image_free(data);
			}

			m_BlockTextureLayers[id] = layer;
			m_BlockTexture->SetLayerData(mergedData, textureSize, layer);
			layer++;
			delete[] mergedData;
		}
	}

}
