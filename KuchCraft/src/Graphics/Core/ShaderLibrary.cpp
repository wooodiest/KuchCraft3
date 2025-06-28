#include "kcpch.h"
#include "Graphics/Core/ShaderLibrary.h"

namespace KuchCraft {

	ShaderLibrary::ShaderLibrary(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "ShaderLibrary name cannot be empty!");
		m_Name = name;
	}

	void ShaderLibrary::AddShader(const Ref<Shader>& shader)
	{
		KC_CORE_ASSERT(shader, "Shader cannot be null!");

		if (Exists(shader->GetName()))
		{
			KC_CORE_WARN("Shader with name '{}' already exists. Overwriting.", shader->GetName());
		}

		m_Shaders[shader->GetName()] = shader;
		KC_CORE_INFO("Shader '{}' added to library {}.", shader->GetName(), m_Name);
	}

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path& path, const std::string& name)
	{
		Ref<Shader> shader = Shader::Create(this, path, name);
		AddShader(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& source, const std::string& name)
	{
		Ref<Shader> shader = Shader::Create(this, source, name);
		AddShader(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name) const
	{
		KC_CORE_ASSERT(!name.empty(), "Shader name cannot be empty.");

		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
		{
			return it->second;
		}
		else
		{
			KC_CORE_WARN("Shader '{}' not found in library '{}'", name, m_Name);
			return nullptr;
		}
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		KC_CORE_ASSERT(!name.empty(), "Shader name cannot be empty.");

		return m_Shaders.find(name) != m_Shaders.end();
	}

	bool ShaderLibrary::Remove(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "Shader name cannot be empty.");

		Ref<Shader> shader = Get(name);
		if (shader)
		{
			m_Shaders.erase(name);
			KC_CORE_INFO("Shader '{}' removed from library '{}'", name, m_Name);
			return true;
		}
		else
		{
			KC_CORE_WARN("Shader '{}' not found in library '{}'", name, m_Name);
			return false;
		}
	}

	void ShaderLibrary::Clear()
	{
		m_Shaders.clear();
		KC_CORE_INFO("All shaders cleared from library '{}'", m_Name);
	}

	void ShaderLibrary::Reload(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "Shader name cannot be empty.");
		auto it = m_Shaders.find(name);
		if (it != m_Shaders.end())
		{
			if (it->second->Reload())
			{
				KC_CORE_INFO("Shader '{}' reloaded successfully.", name);
			}
			else
			{
				KC_CORE_ERROR("Failed to reload shader '{}'", name);
			}
		}
		else
		{
			KC_CORE_WARN("Shader '{}' not found in library '{}'", name, m_Name);
		}
	}

	void ShaderLibrary::ReloadAll()
	{
		for (auto& [name, shader] : m_Shaders)
		{
			if (shader->Reload())
			{
				KC_CORE_INFO("Shader '{}' reloaded successfully.", name);
			}
			else
			{
				KC_CORE_ERROR("Failed to reload shader '{}'", name);
			}
		}

		KC_CORE_INFO("All shaders reloaded in library '{}'", m_Name);
	}

	void ShaderLibrary::SetGlobalSubstitution(const std::string& name, const std::string& value)
	{
		KC_CORE_ASSERT(!name.empty(),  "Substitution name cannot be empty.");
		KC_CORE_ASSERT(!value.empty(), "Substitution value cannot be empty.");

		m_GlobalSubstitutions[name] = value;
	}

	void ShaderLibrary::RemoveGlobalSubstitution(const std::string& name)
	{
		KC_CORE_ASSERT(!name.empty(), "Substitution name cannot be empty.");

		auto it = m_GlobalSubstitutions.find(name);
		if (it != m_GlobalSubstitutions.end())
			m_GlobalSubstitutions.erase(it);
		else
		{
			KC_CORE_WARN("Substitution '{}' not found in library '{}'", name, m_Name);
		}
	}

	void ShaderLibrary::ClearGlobalSubstitutions()
	{
		m_GlobalSubstitutions.clear();
		KC_CORE_INFO("All global substitutions cleared in library '{}'", m_Name);
	}

}
