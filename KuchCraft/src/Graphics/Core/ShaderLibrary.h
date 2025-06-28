#pragma once

#include "Graphics/Core/Shader.h"

namespace KuchCraft {

	class ShaderLibrary
	{
	public:
		const std::map<std::string, std::string>& GetGlobalSubstitutions() const { return m_GlobalSubstitutions; }

	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
		std::map<std::string, std::string> m_GlobalSubstitutions;

	};

}