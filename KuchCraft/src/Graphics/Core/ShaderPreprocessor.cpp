#include "kcpch.h"
#include "Graphics/Core/ShaderPreprocessor.h"

#include "Graphics/Core/ShaderLibrary.h"

namespace KuchCraft {

	std::map<ShaderType, std::string> ShaderPreprocessor::Process(const std::string& source, const std::filesystem::path& parentPath, ShaderLibrary* shaderLibrary, const std::unordered_map<std::string, std::string>& localSubstitutions)
	{
		m_Library = shaderLibrary;
		m_IncludeStack .clear();
		m_Substitutions.clear();
		m_DefinesMap   .clear();

		std::string data = source;
		data = ProcessIncludes(data, parentPath, m_IncludeStack);

		if (m_Library)
		{
			for (const auto& [k, v] : m_Library->GetGlobalSubstitutions())
				m_Substitutions[k] = v;
		}

		for (const auto& [k, v] : localSubstitutions)
			m_Substitutions[k] = v;

		std::unordered_set<std::string> visitedKeys;
		data = SubstituteValues(data, m_Substitutions, visitedKeys);

		return ExtractShaderSources(data);
	}

	static ShaderVariableType ParseShaderVariableType(const std::string& typeStr)
	{
		if (typeStr == "float")       return ShaderVariableType::Float;
		if (typeStr == "vec2")        return ShaderVariableType::Float2;
		if (typeStr == "vec3")        return ShaderVariableType::Float3;
		if (typeStr == "vec4")        return ShaderVariableType::Float4;
		if (typeStr == "int")         return ShaderVariableType::Int;
		if (typeStr == "ivec2")       return ShaderVariableType::Int2;
		if (typeStr == "ivec3")       return ShaderVariableType::Int3;
		if (typeStr == "ivec4")       return ShaderVariableType::Int4;
		if (typeStr == "uint")        return ShaderVariableType::Uint;
		if (typeStr == "bool")        return ShaderVariableType::Bool;
		if (typeStr == "mat3")        return ShaderVariableType::Mat3;
		if (typeStr == "mat4")        return ShaderVariableType::Mat4;
		if (typeStr == "sampler2D")   return ShaderVariableType::Sampler2D;
		if (typeStr == "samplerCube") return ShaderVariableType::SamplerCube;

		return ShaderVariableType::None;
	}

	std::map<ShaderType, std::vector<ShaderVariable>> ShaderPreprocessor::ProcessVariables(const std::map<ShaderType, std::string>& sources)
	{
		std::map<ShaderType, std::vector<ShaderVariable>> result;
		static std::regex layoutRegex(R"((layout\s*\(\s*location\s*=\s*(\d+)\s*\)\s*)?(in|out|uniform)\s+(\w+)\s+(\w+)\s*;)");

		for (const auto& [type, source] : sources)
		{
			if (source.empty())
			{
				KC_CORE_WARN("Shader source for type {} is empty, skipping processing.", ToString(type));
				continue;
			}

			std::istringstream stream(source);
			std::string line;

			while (std::getline(stream, line))
			{
				std::smatch match;

				if (std::regex_search(line, match, layoutRegex))
				{
					ShaderVariable var;

					const std::string& locationStr  = match[1];
					const std::string& locationNum  = match[2];
					const std::string& qualifierStr = match[3];
					const std::string& typeStr      = match[4];
					const std::string& nameStr      = match[5];

					var.Name = nameStr;

					var.Qualifier = [&]() {
						if (qualifierStr == "in")      return ShaderVariableQualifier::In;
						if (qualifierStr == "out")     return ShaderVariableQualifier::Out;
						if (qualifierStr == "uniform") return ShaderVariableQualifier::Uniform;
						return ShaderVariableQualifier::None;
					}();

					var.Type = ParseShaderVariableType(typeStr);

					if (match[2].matched)
						var.Location = std::stoi(locationNum);
					else
						var.Location = -1;

					result[type].push_back(var);
				}
			}
		}

		return result;
	}

	std::map<ShaderType, std::vector<ShaderUniformBlock>> ShaderPreprocessor::ProcessUniformBlocks(const std::map<ShaderType, std::string>& sources)
	{
		std::map<ShaderType, std::vector<ShaderUniformBlock>> result;

		static std::regex blockHeaderRegex(R"(layout\s*\(\s*(.*?)\s*\)\s*uniform\s+(\w+)\s*\{)");
		static std::regex blockSimpleRegex(R"(uniform\s+(\w+)\s*\{)");
		static std::regex blockEndRegex   (R"(\};)");
		static std::regex memberRegex     (R"((\w+)\s+(\w+)\s*;)");

		for (const auto& [type, source] : sources)
		{
			if (source.empty())
				continue;

			std::istringstream stream(source);
			std::string line;
			bool insideBlock = false;

			ShaderUniformBlock currentBlock;

			while (std::getline(stream, line))
			{
				std::smatch match;

				if (!insideBlock)
				{
					if (std::regex_search(line, match, blockHeaderRegex))
					{
						insideBlock = true;
						currentBlock = ShaderUniformBlock();
						const std::string& layoutSpec = match[1];
						currentBlock.Name = match[2];

						std::istringstream layoutStream(layoutSpec);
						std::string token;
						while (std::getline(layoutStream, token, ','))
						{
							token = Utils::Trim(token);
							if (token.starts_with("binding"))
							{
								size_t eq = token.find('=');
								if (eq != std::string::npos)
								{
									std::string val = Utils::Trim(token.substr(eq + 1));
									currentBlock.Binding = std::stoi(val);
								}
							}
							else
							{
								currentBlock.Layout = token;
							}
						}
					}
					else if (std::regex_search(line, match, blockSimpleRegex))
					{
						insideBlock          = true;
						currentBlock         = ShaderUniformBlock();
						currentBlock.Name    = match[1];
						currentBlock.Layout  = "std140";
						currentBlock.Binding = -1;
					}
				}
				else
				{
					if (std::regex_search(line, match, blockEndRegex))
					{
						insideBlock = false;
						result[type].push_back(currentBlock);
					}
					else if (std::regex_search(line, match, memberRegex))
					{
						const std::string& typeStr = match[1];
						const std::string& nameStr = match[2];

						ShaderVariable member;
						member.Name      = nameStr;
						member.Qualifier = ShaderVariableQualifier::None;
						member.Type = ParseShaderVariableType(typeStr);

						currentBlock.Members.push_back(member);
					}
				}
			}
		}

		return result;
	}

	std::string ShaderPreprocessor::ProcessIncludes(const std::string& source, const std::filesystem::path& parentPath, std::unordered_set<std::filesystem::path>& includeStack)
	{
		std::stringstream result;
		std::istringstream stream(source);
		std::string line;

		while (std::getline(stream, line))
		{
			std::string trimmed = Utils::TrimStart(line);

			if (trimmed.starts_with("#include"))
			{
				size_t start = trimmed.find_first_of('"') + 1;
				size_t end = trimmed.find_last_of('"');
				if (start == std::string::npos || end == std::string::npos || start >= end)
				{
					KC_CORE_ERROR("Invalid #include syntax in shader: {}", line);
					continue;
				}

				std::string includeFileName = trimmed.substr(start, end - start);
				std::filesystem::path includePath = parentPath.parent_path() / includeFileName;
				try
				{
					includePath = std::filesystem::canonical(includePath);
				}
				catch (const std::filesystem::filesystem_error&) {
					KC_CORE_ERROR("Invalid include path: {}", includePath.string());
					continue;
				}

				if (includeStack.contains(includePath))
				{
					KC_CORE_WARN("Skipping recursive #include: {}", includePath.string());
					continue;
				}

				if (!std::filesystem::exists(includePath))
				{
					KC_CORE_ERROR("Included file not found: {}", includePath.string());
					continue;
				}

				includeStack.insert(includePath);
				std::string includedSource = Utils::ReadFile(includePath);
				includedSource = ProcessIncludes(includedSource, includePath, includeStack);

				result << includedSource << "\n";
			}
			else
			{
				result << line << "\n";
			}
		}

		return result.str();
	}

	std::string ShaderPreprocessor::SubstituteValues(const std::string& source, const std::unordered_map<std::string, std::string>& valueMap, std::unordered_set<std::string>& visitedKeys)
	{
		std::string result = source;
		const std::string markerStart = "#value(";
		const std::string markerEnd = ")";

		size_t pos = 0;
		while ((pos = result.find(markerStart, pos)) != std::string::npos)
		{
			size_t end = result.find(markerEnd, pos);
			if (end == std::string::npos)
				break;

			size_t nameStart = pos + markerStart.length();
			std::string key = result.substr(nameStart, end - nameStart);

			if (visitedKeys.contains(key))
			{
				KC_CORE_ERROR("Cyclic #value reference: '{}'", key);
				result.replace(pos, end - pos + markerEnd.length(), "");
				continue;
			}

			auto it = valueMap.find(key);
			if (it == valueMap.end())
			{
				KC_CORE_WARN("Unknown #value key: '{}'", key);
				result.replace(pos, end - pos + markerEnd.length(), "");
				continue;
			}

			visitedKeys.insert(key);
			std::string resolved = SubstituteValues(it->second, valueMap, visitedKeys);
			visitedKeys.erase(key);

			result.replace(pos, end - pos + markerEnd.length(), resolved);
			pos += resolved.length();
		}
		return result;
	}

	std::unordered_map<std::string, std::string> ShaderPreprocessor::BuildDefinesMap(const std::string& source)
	{
		std::unordered_map<std::string, std::string> result;

		std::istringstream stream(source);
		std::string line;
		while (std::getline(stream, line))
		{
			std::string trimmed = Utils::TrimStart(line);

			if (trimmed.starts_with("#define"))
			{
				std::istringstream defStream(trimmed);
				std::string directive, name, value;
				defStream >> directive >> name;
				std::getline(defStream, value);
				value = Utils::Trim(value);
				result[name] = value;
			}
			else if (trimmed.starts_with("#undef"))
			{
				std::istringstream defStream(trimmed);
				std::string directive, name;
				defStream >> directive >> name;
				result.erase(name);
			}
		}

		m_DefinesMap = result;
		return result;
	}

	std::string ShaderPreprocessor::EvaluateConditions(const std::string& source, const std::unordered_map<std::string, std::string>& macroMap)
	{
		struct IfBlock {
			bool parentEnabled = true;
			bool thisBranchEnabled = false;
			bool anyBranchMatched = false;
			bool inElse = false;
		};

		std::istringstream input(source);
		std::ostringstream output;
		std::stack<IfBlock> stack;

		std::string line;
		while (std::getline(input, line))
		{
			std::string trimmed = Utils::TrimStart(line);

			auto getCurrentEnabled = [&]() -> bool {
				return stack.empty() ? true : stack.top().parentEnabled && stack.top().thisBranchEnabled;
			};

			if (trimmed.starts_with("#ifdef"))
			{
				std::string macro = Utils::Trim(trimmed.substr(6));
				bool cond = macroMap.contains(macro);
				bool parent = getCurrentEnabled();
				stack.push({ parent, cond, cond, false });
				continue;
			}
			else if (trimmed.starts_with("#ifndef"))
			{
				std::string macro = Utils::Trim(trimmed.substr(7));
				bool cond = !macroMap.contains(macro);
				bool parent = getCurrentEnabled();
				stack.push({ parent, cond, cond, false });
				continue;
			}
			else if (trimmed.starts_with("#if"))
			{
				std::string expr = Utils::Trim(trimmed.substr(3));
				bool cond = EvaluateConditionExpression(expr, macroMap);
				bool parent = getCurrentEnabled();
				stack.push({ parent, cond, cond, false });
				continue;
			}
			else if (trimmed.starts_with("#elif"))
			{
				if (stack.empty())
				{
					KC_CORE_ERROR("Unexpected #elif with no matching #if");
					continue;
				}

				std::string expr = Utils::Trim(trimmed.substr(5));
				IfBlock& top = stack.top();

				if (top.inElse)
				{
					KC_CORE_ERROR("#elif after #else is not allowed");
					continue;
				}

				if (!top.anyBranchMatched && EvaluateConditionExpression(expr, macroMap))
				{
					top.thisBranchEnabled = true;
					top.anyBranchMatched  = true;
				}
				else
				{
					top.thisBranchEnabled = false;
				}

				continue;
			}
			else if (trimmed.starts_with("#else"))
			{
				if (stack.empty())
				{
					KC_CORE_ERROR("Unexpected #else with no matching #if");
					continue;
				}

				IfBlock& top = stack.top();
				if (top.inElse)
				{
					KC_CORE_ERROR("Multiple #else blocks in same #if");
					continue;
				}
				top.thisBranchEnabled = !top.anyBranchMatched;
				top.inElse = true;
				continue;
			}
			else if (trimmed.starts_with("#endif"))
			{
				if (stack.empty())
				{
					KC_CORE_ERROR("Unexpected #endif with no matching #if");
					continue;
				}
				stack.pop();
				continue;
			}

			if (getCurrentEnabled())
				output << line << "\n";
		}

		if (!stack.empty())
			KC_CORE_ERROR("Unclosed #if block(s) detected");

		return output.str();
	}

	bool ShaderPreprocessor::EvaluateConditionExpression(const std::string& expr, const std::unordered_map<std::string, std::string>& macros)
	{
		std::smatch match;
		static const std::regex pattern(R"(\s*(\w+)\s*(==|!=|>=|<=|>|<)\s*(\w+)\s*)");

		if (std::regex_match(expr, match, pattern))
		{
			std::string lhs = match[1];
			std::string op  = match[2];
			std::string rhs = match[3];

			std::string lhsValue = macros.contains(lhs) ? macros.at(lhs) : "0";
			std::string rhsValue = rhs;

			try {
				int lhsInt = std::stoi(lhsValue);
				int rhsInt = std::stoi(rhsValue);

				if (op == "==") return lhsInt == rhsInt;
				if (op == "!=") return lhsInt != rhsInt;
				if (op == ">")  return lhsInt > rhsInt;
				if (op == "<")  return lhsInt < rhsInt;
				if (op == ">=") return lhsInt >= rhsInt;
				if (op == "<=") return lhsInt <= rhsInt;
			}
			catch (...) {
				// fallback to string comparison
				if (op == "==") return lhsValue == rhsValue;
				if (op == "!=") return lhsValue != rhsValue;
			}

			KC_CORE_WARN("Unsupported operator or conversion in #if expression: '{}'", expr);
			return false;
		}
		else
		{
			// fallback: #if MACRO
			auto it = macros.find(expr);
			if (it == macros.end()) return false;
			const std::string& val = it->second;
			return val != "0" && val != "false";
		}
	}

	std::map<ShaderType, std::string> ShaderPreprocessor::ExtractShaderSources(const std::string& source)
	{
		std::map<ShaderType, std::string> result;

		std::istringstream stream(source);
		std::stringstream  currentShader;

		std::string line;
		ShaderType currentType = ShaderType::None;

		auto flushCurrent = [&]() {
			if (currentType != ShaderType::None)
				result[currentType] = currentShader.str();
			currentShader.str("");
			currentShader.clear();
		};

		while (std::getline(stream, line))
		{
			std::string trimmed = Utils::Trim(line);

			if (trimmed.starts_with("###"))
			{
				flushCurrent();

				std::string typeStr = Utils::Trim(trimmed.substr(3));
				if (auto typeOpt = FromString<ShaderType>(typeStr); typeOpt.has_value())
					currentType = *typeOpt;
				else
				{
					currentType = ShaderType::None;
					KC_CORE_WARN("Unknown shader type section: {}", typeStr);
				}
			}
			else if (currentType != ShaderType::None)
			{
				currentShader << line << "\n";
			}
		}

		flushCurrent();
		return result;
	}

}
