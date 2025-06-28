#include "kcpch.h"
#include "Graphics/Core/Shader.h"

#include "Graphics/Core/ShaderLibrary.h"

#include <glad/glad.h>

namespace KuchCraft {

	Shader::Shader(const std::string& name, const std::string& source, const std::filesystem::path& path, Weak<ShaderLibrary> shaderLibrary)
		: m_Name(name), m_Source(source), m_Path(path), m_Library(shaderLibrary)
	{
		Compile(m_Source);
	}

	Shader::~Shader()
	{
		if (IsValid())
			glDeleteProgram(m_RendererID);
	}

	Ref<Shader> Shader::Create(const std::filesystem::path& path, const std::string& name)
	{
		return Create(Weak<ShaderLibrary>(), path, name);
	}

	Ref<Shader> Shader::Create(const std::string& source, const std::string& name)
	{
		return Ref<Shader>(new Shader(name, source));
	}

	Ref<Shader> Shader::Create(Weak<ShaderLibrary> shaderLibrary, const std::filesystem::path& path, const std::string& name)
	{
		if (path.extension() != ".glsl" && path.extension() != ".shader")
		{
			KC_CORE_ERROR("Shader file must have .glsl or .shader extension: {}", path.string());
			return nullptr;
		}

		std::string shaderName = name;
		if (shaderName.empty())
			shaderName = path.filename().replace_extension("").string();

		std::string source = Utils::ReadFile(path);

		return Ref<Shader>(new Shader(shaderName, source, path, shaderLibrary));
	}

	Ref<Shader> Shader::Create(Weak<ShaderLibrary> shaderLibrary, const std::string& source, const std::string& name)
	{
		return Ref<Shader>(new Shader(name, source, std::filesystem::path(), shaderLibrary));
	}

	void Shader::SetLocalSubstitution(const std::string& name, const std::string& value)
	{
		KC_CORE_ASSERT(!name.empty(),  "Substitution name cannot be empty.");
		KC_CORE_ASSERT(!value.empty(), "Substitution value cannot be empty.");

		m_LocalSubstitutions[name] = value;
	}

	void Shader::RemoveLocalSubstitution(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "Substitution name cannot be empty.");

		auto it = m_LocalSubstitutions.find(name);
		if (it != m_LocalSubstitutions.end())
			m_LocalSubstitutions.erase(it);
		else
			KC_CORE_WARN("Substitution '{}' not found in shader '{}'", name, m_Name);
	}

	void Shader::ClearLocalSubstitutions()
	{
		m_LocalSubstitutions.clear();
	}

	bool Shader::Reload()
	{
		std::string source = m_Source;
		if (std::filesystem::exists(m_Path))
			source = Utils::ReadFile(m_Path);
		
		bool success = Compile(source);
		if (success)
			m_Source = source;
		
		return success;
	}

	void Shader::SetInt(const std::string& name, int value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform1i(location, value);
	}

	void Shader::SetIntArray(const std::string& name, int* values, int count)
	{
		int location = GetUniformLocation(name);
		glUniform1iv(location, count, values);
	}

	void Shader::SetFloat(const std::string& name, float value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform1f(location, value);
	}

	void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform2f(location, value.x, value.y);
	}

	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		int location = GetUniformLocation(name);
		if (location != -1)
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	static GLenum ShaderTypeToGLenum(ShaderType type)
	{
		switch (type)
		{
			case ShaderType::Vertex:         return GL_VERTEX_SHADER;
			case ShaderType::Fragment:       return GL_FRAGMENT_SHADER;
			case ShaderType::Geometry:       return GL_GEOMETRY_SHADER;
			case ShaderType::TessControl:    return GL_TESS_CONTROL_SHADER;
			case ShaderType::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
		}

		KC_CORE_ASSERT(false, "Unknown ShaderType");
		return 0;
	}

	bool Shader::Compile(const std::string& source)
	{
		auto shaderSources = m_Preprocessor.Process(source, std::filesystem::path(), m_Library, m_LocalSubstitutions);

		std::vector<GLuint> compiledShaders;
		GLuint program = glCreateProgram();

		for (const auto& [type, source] : shaderSources)
		{
			if (source.empty())
			{
				KC_CORE_WARN("Shader source for type {} is empty, skipping compilation.", ToString(type));
				continue;
			}
			if (type == ShaderType::None)
			{
				KC_CORE_WARN("Shader type is None, skipping compilation.");
				continue;
			}

			GLuint shader = glCreateShader(ShaderTypeToGLenum(type));
			const char* src = source.c_str();
			glShaderSource(shader, 1, &src, nullptr);
			glCompileShader(shader);

			GLint success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLint length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

				std::vector<char> infoLog(length);
				glGetShaderInfoLog(shader, length, nullptr, infoLog.data());

				KC_CORE_ERROR("Failed to compile shader of type {}: {}", ToString(type), infoLog.data());
				glDeleteShader(shader);
				return false;
			}
			glAttachShader(program, shader);
			compiledShaders.push_back(shader);
		}

		if (compiledShaders.empty())
		{
			KC_CORE_ERROR("No valid shaders compiled for program '{}'", m_Name);
			glDeleteProgram(program);
			return false;
		}

		glLinkProgram(program);
		GLint linkSuccess;
		glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
		if (!linkSuccess)
		{
			GLint length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

			std::vector<char> infoLog(length);
			glGetProgramInfoLog(program, length, nullptr, infoLog.data());

			KC_CORE_ERROR("Failed to link shader program '{}': {}", m_Name, infoLog.data());
			glDeleteProgram(program);

			for (GLuint shader : compiledShaders)
				glDeleteShader(shader);

			return false;
		}

		for (GLuint shader : compiledShaders)
		{
			glDetachShader(program, shader);
			glDeleteShader(shader);
		}

		if (GLAD_GL_KHR_debug)
			glObjectLabel(GL_PROGRAM, program, static_cast<GLsizei>(m_Name.length()), m_Name.c_str());

		if (IsValid())
		{
			glDeleteProgram(m_RendererID);
			m_RendererID = 0;
		}

		m_RendererID = program;
		m_UniformLocations.clear();
		KC_CORE_INFO("Successfully compiled shader '{}'", m_Name);

		return true;
	}

	int Shader::GetUniformLocation(const std::string& name)
	{
		KC_CORE_ASSERT(IsValid(), "Shader is not valid.");
		KC_CORE_ASSERT(!name.empty(), "Uniform name cannot be empty.");

		auto it = m_UniformLocations.find(name);
		if (it != m_UniformLocations.end())
			return it->second;

		GLuint location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1)
		{
			KC_CORE_WARN("Uniform '{}' not found in shader '{}'", name, m_Name);
			return -1;
		}

		m_UniformLocations[name] = location;
		return location;
	}

}
