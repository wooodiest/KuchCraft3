#include "kcpch.h"
#include "Scene/AssetManager.h"

namespace KuchCraft {

	AssetManager::AssetManager(Config config, const std::filesystem::path& dataPackPath)
		: m_Config(config), m_DataPackPath(dataPackPath)
	{

	}

	AssetManager::~AssetManager()
	{

	}

	AssetHandle AssetManager::Load(const std::filesystem::path& filepath)
	{
		if (m_AssetsPaths.find(filepath) != m_AssetsPaths.end())
			return m_AssetsPaths[filepath];

		if (!std::filesystem::exists(filepath))
		{
			KC_CORE_WARN("File does not exist: {}", filepath.string());
			return AssetHandle();
		}

		std::ifstream file(filepath);
		if (!file.is_open())
		{
			KC_CORE_ERROR("Failed to open file: {}", filepath.string());
			return AssetHandle();
		}

		nlohmann::json assetJson;
		try
		{
			file >> assetJson;
			file.close();
		}
		catch (std::exception& e)
		{
			KC_CORE_ERROR("Failed to parse JSON: {}", e.what());
			return AssetHandle();
		}

		AssetHandle assetHandle;

		if (assetJson.contains("Type"))
			assetHandle.Type = FromString<AssetType>(assetJson["Type"].get<std::string>()).value_or(AssetType::None);
		else
		{
			KC_CORE_ERROR("Asset JSON does not contain 'Type' field: {}", filepath.string());
			return AssetHandle();
		}

		if (assetHandle.Type == AssetType::None)
		{
			KC_CORE_ERROR("Unsupported asset type in JSON: {}", filepath.string());
			return AssetHandle();
		}

		if (assetJson.contains("UUID"))
			assetHandle.ID = assetJson["UUID"].get<UUID>();

		if (assetHandle.ID == 0)
		{
			KC_CORE_ERROR("Asset JSON does not contain 'UUID' field: {}", filepath.string());
			return AssetHandle();
		}
		
		if (assetJson.contains("Name"))
			assetHandle.Name = assetJson["Name"].get<std::string>();
		else
			assetHandle.Name = filepath.filename().string();

		bool good = true;
		switch (assetHandle.Type)
		{
			case AssetType::Texture2D: good = LoadTexture2D(assetHandle, assetJson); break;
		}

		if (!good)
		{
			KC_CORE_ERROR("Failed to load asset: {}", filepath.string());
			return AssetHandle();
		}

		return assetHandle;
	}

	AssetHandle AssetManager::Load(const std::string& name)
	{
		const std::filesystem::path assetPath = m_DataPackPath / "assets" / std::filesystem::path(name);
		return Load(assetPath);
	}

	void AssetManager::LoadAll()
	{
		const std::filesystem::path assetsDir = m_DataPackPath / "assets";
		for (const auto& entry : std::filesystem::directory_iterator(assetsDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
				Load(entry.path());
		}
	}

	bool AssetManager::LoadTexture2D(AssetHandle& handle, const nlohmann::json& assetJson)
	{
		std::filesystem::path filePath;
		if (assetJson.contains("FilePath"))
			filePath = m_DataPackPath / assetJson["FilePath"].get<std::filesystem::path>();

		TextureSpecification spec;
		if (assetJson.contains("Specification"))
		{
			auto& specJson = assetJson["Specification"];
			if (specJson.contains("Width"))
				spec.Width = specJson["Width"].get<int>();
			if (specJson.contains("Height"))
				spec.Height = specJson["Height"].get<int>();
			if (specJson.contains("Format"))
				spec.Format = FromString<TextureFormat>(specJson["Format"].get<std::string>()).value_or(TextureFormat::RGBA);
			if (specJson.contains("Filter"))
				spec.Filter = FromString<TextureFilter>(specJson["Filter"].get<std::string>()).value_or(TextureFilter::Nearest);
			if (specJson.contains("Wrap"))
				spec.Wrap = FromString<TextureWrap>(specJson["Wrap"].get<std::string>()).value_or(TextureWrap::Clamp);
		}

		Ref<Texture2D> texture = Texture2D::Create(filePath, spec);
		if (texture->IsValid())
		{
			m_Textures2D[handle.ID] = texture;
		}
		else
		{
			KC_CORE_ERROR("Failed to load texture: {}", handle.Name);
			return false;
		}

		return true;
	}

}
