#pragma once

#include "Graphics/Core/Shader.h"

namespace KuchCraft {

	class ShaderLibrary
	{
	public:
		ShaderLibrary()  = default;
		ShaderLibrary(const std::string& name);
		~ShaderLibrary() = default;

		void AddShader(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::filesystem::path& path, const std::string& name = std::string());
		Ref<Shader> Load(const std::string& source, const std::string& name);

		Ref<Shader> Get(const std::string& name) const;
		bool Exists(const std::string& name) const;
		bool Remove(const std::string& name);
		void Clear();

		void Reload(const std::string& name);
		void ReloadAll();

		void SetPath(const std::filesystem::path& path) { m_Path = path; }
		const std::filesystem::path& GetPath() const { return m_Path; }

		const std::unordered_map<std::string, Ref<Shader>>& GetShaders() const { return m_Shaders; }

		const std::map<std::string, std::string>& GetGlobalSubstitutions() const { return m_GlobalSubstitutions; }
		void SetGlobalSubstitution(const std::string& name, const std::string& value);
		void RemoveGlobalSubstitution(const std::string& name);
		void ClearGlobalSubstitutions();

		const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name = "DefaultShaderLibrary";
		std::filesystem::path m_Path;
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		std::map<std::string, std::string> m_GlobalSubstitutions;

		KC_DISALLOW_COPY(ShaderLibrary);
		KC_DISALLOW_MOVE(ShaderLibrary);
	};

}