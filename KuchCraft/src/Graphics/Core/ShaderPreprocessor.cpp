#include "kcpch.h"
#include "Graphics/Core/ShaderPreprocessor.h"

#include "Graphics/Core/ShaderLibrary.h"

namespace KuchCraft {

	std::map<ShaderType, std::string> ShaderPreprocessor::Process(const std::string& source, const std::filesystem::path& parentPath, Weak<ShaderLibrary> shaderLibrary, const std::unordered_map<std::string, std::string>& localSubstitutions)
	{
		m_Library = shaderLibrary;
		m_IncludeStack .clear();
		m_Substitutions.clear();
		m_DefinesMap   .clear();

		std::string data = source;
		data = ProcessIncludes(data, parentPath, m_IncludeStack);

		if (Ref<ShaderLibrary> lib = m_Library.lock())
		{
			for (const auto& [k, v] : lib->GetGlobalSubstitutions())
				m_Substitutions[k] = v;
		}

		for (const auto& [k, v] : localSubstitutions)
			m_Substitutions[k] = v;

		std::unordered_set<std::string> visitedKeys;
		data = SubstituteValues(data, m_Substitutions, visitedKeys);

		return ExtractShaderSources(data);
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
		const std::string markerStart = "#pragma value(";
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
				KC_CORE_ERROR("Cyclic #pragma value reference: '{}'", key);
				result.replace(pos, end - pos + markerEnd.length(), "");
				continue;
			}

			auto it = valueMap.find(key);
			if (it == valueMap.end())
			{
				KC_CORE_WARN("Unknown #pragma value key: '{}'", key);
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

				std::string typeStr = trimmed.substr(3);
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
