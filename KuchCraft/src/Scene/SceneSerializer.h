#pragma once

#include "Scene/Scene.h"

namespace KuchCraft {

	class SceneSerializer
	{
	public:
		SceneSerializer(Scene* scene);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

		inline static std::string DefaultExtension = ".kscene";

	private:
		Scene* m_Scene;
	};

}