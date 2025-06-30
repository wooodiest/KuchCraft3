#pragma once

#include "Graphics/Core/ShaderPreprocessor.h"
#include "Graphics/Core/VertexBuffer.h"

namespace KuchCraft
{
	enum class ShaderType
	{
		None = 0,
		Vertex,
		Fragment,
		Geometry,
		TessControl,
		TessEvaluation,
	};

	class ShaderLibrary;

	class Shader
	{
	public:
		~Shader();

		static Ref<Shader> Create(const std::filesystem::path& path, const std::string& name = std::string());
		static Ref<Shader> Create(const std::string& source, const std::string& name);

		static Ref<Shader> Create(ShaderLibrary* shaderLibrary, const std::filesystem::path& path, const std::string& name = std::string());
		static Ref<Shader> Create(ShaderLibrary* shaderLibrary, const std::string& source, const std::string& name);

		bool IsValid() const { return m_RendererID != 0; }
		bool Reload();

		void Bind()   const;
		void Unbind() const;

		void SetInt(const std::string& name, int value);
		void SetIntArray(const std::string& name, int* values, int count);

		void SetFloat (const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);

		void SetMat3(const std::string& name, const glm::mat3& value);
		void SetMat4(const std::string& name, const glm::mat4& value);

		const std::unordered_map<std::string, std::string>& GetLocalSubstitutions() const { return m_LocalSubstitutions; }
		void SetLocalSubstitution(const std::string& name, const std::string& value);
		void RemoveLocalSubstitution(const std::string& name);
		void ClearLocalSubstitutions();

		RendererID GetRendererID()     const { return m_RendererID; }
		const std::string& GetName()   const { return m_Name;   }
		const std::string& GetSource() const { return m_Source; }
		const std::filesystem::path& GetPath() const { return m_Path; }

		const std::map<ShaderType, std::string>&                     GetShaderSources() const { return m_ShaderSources; }
		const std::map<ShaderType, std::vector<ShaderVariable>>&     GetVariables()     const { return m_Variables;     }
		const std::map<ShaderType, std::vector<ShaderUniformBlock>>& GetUniformBlocks() const { return m_UniformBlocks; }

		BufferLayout GetVertexInputLayout() const;

		void LogLayout() const;

	private:
		bool Compile(const std::string& source);
		void SetUniformLocations();
		int GetUniformLocation(const std::string& name);

	private:
		RendererID m_RendererID = 0;

		std::string m_Name;
		std::string m_Source;
		std::filesystem::path m_Path;

		std::map<ShaderType, std::string> m_ShaderSources;

		ShaderLibrary* m_Library;
		std::unordered_map<std::string, std::string> m_LocalSubstitutions;
		ShaderPreprocessor m_Preprocessor;

		std::unordered_map<std::string, int> m_UniformLocations;
		std::map<ShaderType, std::vector<ShaderVariable>>     m_Variables;
		std::map<ShaderType, std::vector<ShaderUniformBlock>> m_UniformBlocks;

	private:
		Shader(const std::string& name, const std::string& source, const std::filesystem::path& path = std::filesystem::path(), ShaderLibrary* shaderLibrary = nullptr);

		KC_DISALLOW_COPY(Shader);
		KC_DISALLOW_MOVE(Shader);

	};
}