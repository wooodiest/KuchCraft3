#pragma once

#include "Core/Config.h"
#include "Graphics/Core/Core.h"

namespace KuchCraft {

	enum class AssetType
	{
		Texture2D,
		None
	};

	struct AssetHandle
	{
		UUID ID = 0;

		bool IsValid() const { return ID != 0; }
	};

	class AssetManager
	{
	public:
		AssetManager(Config config, const std::filesystem::path& dataPackPath);
		~AssetManager();

		AssetType GetType(const AssetHandle& handle) const {
			auto it = m_AssetsTypes.find(handle.ID);
			if (it != m_AssetsTypes.end())
				return it->second;
			return AssetType::None;
		}

		std::string GetName(const AssetHandle& handle) const {
			auto it = m_AssetsNames.find(handle.ID);
			if (it != m_AssetsNames.end())
				return it->second;
			return "Unnamed asset";
		}

		AssetHandle Load(const std::filesystem::path& filepath);
		AssetHandle Load(const std::string& name);
		void LoadAll();

		Ref<Texture2D> GetTexture2D(const AssetHandle& handle) const
		{
			auto it = m_Textures2D.find(handle.ID);
			if (it != m_Textures2D.end())
				return it->second;

			return nullptr;
		}

	private:
		bool LoadTexture2D(AssetHandle& handle, const nlohmann::json& assetJson);

	private:
		Config m_Config;
		std::filesystem::path m_DataPackPath;

		std::unordered_map<UUID, AssetType> m_AssetsTypes;
		std::unordered_map<UUID, std::string> m_AssetsNames;
		std::unordered_map<std::filesystem::path, AssetHandle> m_AssetsPaths;

		std::unordered_map<UUID, Ref<Texture2D>> m_Textures2D;
	};

}