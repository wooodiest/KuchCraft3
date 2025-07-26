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
		UUID        ID = 0;
		AssetType   Type = AssetType::None;
		std::string Name = "Unnamed Asset";

		bool IsValid() const { return ID != 0; }
	};

	class AssetManager
	{
	public:
		AssetManager(Config config, const std::filesystem::path& dataPackPath);
		~AssetManager();

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

		std::unordered_map<std::filesystem::path, AssetHandle> m_AssetsPaths;
		std::unordered_map<UUID, Ref<Texture2D>> m_Textures2D;
	};

}