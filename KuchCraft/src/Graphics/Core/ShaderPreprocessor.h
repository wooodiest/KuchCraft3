#pragma once

namespace KuchCraft {

	enum class ShaderVariableQualifier : uint8_t
	{
		None = 0,
		In,
		Out,
		Uniform
	};

	enum class ShaderVariableType : uint8_t
	{
		None = 0,

		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Bool,
		Uint,

		Mat3, Mat4,

		Sampler2D,
		SamplerCube
	};

	struct ShaderVariable
	{
		std::string             Name;
		ShaderVariableQualifier Qualifier = ShaderVariableQualifier::None;
		ShaderVariableType      Type      = ShaderVariableType::None;
		int                     Location  = -1;
	};

	struct ShaderUniformBlock
	{
		std::string Name;
		std::string Layout = "std140";
		int         Binding = -1;
		std::vector<ShaderVariable> Members;
	};

	class ShaderLibrary;
	enum class ShaderType;

	class ShaderPreprocessor
	{
	public:
		ShaderPreprocessor()  = default;
		~ShaderPreprocessor() = default;

		std::map<ShaderType, std::string> Process(const std::string& source, const std::filesystem::path& parentPath,
			ShaderLibrary* shaderLibrary, const std::unordered_map<std::string, std::string>& localSubstitutions);
		
		std::map<ShaderType, std::vector<ShaderVariable>>     ProcessVariables(const std::map<ShaderType, std::string>& sources);
		std::map<ShaderType, std::vector<ShaderUniformBlock>> ProcessUniformBlocks(const std::map<ShaderType, std::string>& sources);

		const auto& GetIncludeStack()  const { return m_IncludeStack;  }
		const auto& GetSubstitutions() const { return m_Substitutions; }
		const auto& GetDefinesMap()    const { return m_DefinesMap;    }

	private:
		std::string ProcessIncludes(const std::string& source, const std::filesystem::path& parentPath, std::unordered_set<std::filesystem::path>& includeStack);
		std::string SubstituteValues(const std::string& source, const std::unordered_map<std::string, std::string>& valueMap, std::unordered_set<std::string>& visitedKeys);
		std::unordered_map<std::string, std::string> BuildDefinesMap(const std::string& source);

		std::string EvaluateConditions(const std::string& source, const std::unordered_map<std::string, std::string>& macroMap);
		bool EvaluateConditionExpression(const std::string& expr, const std::unordered_map<std::string, std::string>& macros);

		std::map<ShaderType, std::string> ExtractShaderSources(const std::string& source);

	private:
		ShaderLibrary* m_Library;
		std::unordered_set<std::filesystem::path>    m_IncludeStack;
		std::unordered_map<std::string, std::string> m_Substitutions;
		std::unordered_map<std::string, std::string> m_DefinesMap;

		KC_DISALLOW_COPY(ShaderPreprocessor);
		KC_DISALLOW_MOVE(ShaderPreprocessor);
	};

}